/*
 * fosphor.c
 *
 * Main fosphor entry point
 *
 * Copyright (C) 2013-2021 Sylvain Munaut
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/*! \addtogroup fosphor
 *  @{
 */

/*! \file fosphor.c
 *  \brief Main fosphor entry point
 */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cl.h"
#include "gl.h"
#include "fosphor.h"
#include "private.h"


struct fosphor *
fosphor_init(void)
{
	struct fosphor *self;
	int rv;

	/* Allocate structure */
	self = malloc(sizeof(struct fosphor));
	if (!self)
		return NULL;

	memset(self, 0, sizeof(struct fosphor));

	/* Init GL/CL sub-states */
	rv = fosphor_gl_init(self);
	if (rv)
		goto error;

	rv = fosphor_cl_init(self);
	if (rv)
		goto error;

	/* Buffers (if needed) */
	if (!(self->flags & FLG_FOSPHOR_USE_CLGL_SHARING))
	{
		self->img_waterfall = malloc(FOSPHOR_FFT_LEN * 1024 * sizeof(float));
		self->img_histogram = malloc(FOSPHOR_FFT_LEN *  128 * sizeof(float));
		self->buf_spectrum  = malloc(2 * 2 * FOSPHOR_FFT_LEN * sizeof(float));

		if (!self->img_waterfall ||
		    !self->img_histogram ||
		    !self->buf_spectrum)
			goto error;
	}

	/* Initial state */
	fosphor_set_fft_window_default(self);
	fosphor_set_power_range(self, 0, 10);

	return self;

	/* Error path */
error:
	fosphor_release(self);
	return NULL;
}

void
fosphor_release(struct fosphor *self)
{
	if (!self)
		return;

	free(self->img_waterfall);
	free(self->img_histogram);
	free(self->buf_spectrum);

	fosphor_cl_release(self);
	fosphor_gl_release(self);

	free(self);
}

int
fosphor_process(struct fosphor *self, void *samples, int len)
{
	return fosphor_cl_process(self, samples, len);
}

void
fosphor_draw(struct fosphor *self, struct fosphor_render *render)
{
	if (fosphor_cl_finish(self) > 0)
		fosphor_gl_refresh(self);
	render->_wf_pos = fosphor_cl_get_waterfall_position(self);
	fosphor_gl_draw(self, render);
}


void
fosphor_set_fft_window_default(struct fosphor *self)
{
	int i;

	/* Default Hamming window (periodic) */
	for (i=0; i<FOSPHOR_FFT_LEN; i++) {
		float ft = (float)FOSPHOR_FFT_LEN;
		float fp = (float)i;
		self->fft_win[i] = (0.54f - 0.46f * cosf((2.0f * 3.141592f * fp) / ft)) * 1.855f;
	}

	fosphor_cl_load_fft_window(self, self->fft_win);
}

void
fosphor_set_fft_window(struct fosphor *self, float *win)
{
	memcpy(self->fft_win, win, sizeof(float) * FOSPHOR_FFT_LEN);
	fosphor_cl_load_fft_window(self, self->fft_win);
}


void
fosphor_set_power_range(struct fosphor *self, int db_ref, int db_per_div)
{
	int db0, db1;
	float k;
	float scale, offset;

	db0 = db_ref - 10*db_per_div;
	db1 = db_ref;

	k = log10f((float)FOSPHOR_FFT_LEN);

	offset = - ( k + ((float)db0 / 20.0f) );
	scale  = 20.0f / (float)(db1 - db0);

	self->power.db_ref     = db_ref;
	self->power.db_per_div = db_per_div;
	self->power.scale      = scale;
	self->power.offset     = offset;

	fosphor_cl_set_histogram_range(self, scale, offset);
}

void
fosphor_set_frequency_range(struct fosphor *self, double center, double span)
{
	self->frequency.center = center;
	self->frequency.span   = span;
}


void
fosphor_render_defaults(struct fosphor_render *render)
{
	render->pos_x  = 0;
	render->pos_y  = 0;
	render->width  = 1024;
	render->height = 1024;

	render->options =
		FRO_LIVE	|
		FRO_MAX_HOLD	|
		FRO_HISTO	|
		FRO_WATERFALL	|
		FRO_LABEL_FREQ	|
		FRO_LABEL_PWR	|
		FRO_LABEL_TIME	|
		FRO_COLOR_SCALE;

	render->histo_wf_ratio = 0.5f;
	render->freq_n_div     = 10;
	render->freq_center    = 0.5f;
	render->freq_span      = 1.0f;
	render->wf_span        = 1.0f;
}

void
fosphor_render_refresh(struct fosphor_render *render)
{
	int disp_spectrum, disp_waterfall;
	int avail, div, over, rsvd, rsvd_lr[2];
	float y_top, y_bot;

	/* Which screen zone ? */
	disp_spectrum  = !!(render->options & (FRO_LIVE | FRO_MAX_HOLD | FRO_HISTO));
	disp_waterfall = !!(render->options & FRO_WATERFALL);

	/* Split the X space */
	rsvd_lr[0] = 10;
	rsvd_lr[1] = 10;

	if (render->options & (FRO_LABEL_PWR | FRO_LABEL_TIME))
		rsvd_lr[0] += 30;

	if (render->options & FRO_COLOR_SCALE)
		rsvd_lr[1] += 10;

	rsvd = rsvd_lr[0] + rsvd_lr[1];

	render->freq_n_div = ((int)(render->width - rsvd) / 80) & ~1;
	if (render->freq_n_div > 10)
		render->freq_n_div = 10;
	if (render->freq_n_div < 2)
		render->freq_n_div = 2;

	avail = render->width - rsvd;
	div   = avail / render->freq_n_div;
	over  = avail - (render->freq_n_div * div);

	render->_x_div = (float)div;
	render->_x[0] = render->pos_x + (float)(rsvd_lr[0]) + (float)(over / 2);
	render->_x[1] = render->_x[0] + (render->freq_n_div * render->_x_div) + 1.0f;
	render->_x_label = render->_x[0] - 5.0f;

	/* Split the Y space */
	y_top = render->pos_y + (float)render->height - 10.0f;
	y_bot = render->pos_y + 10.0f;

	if (disp_spectrum)
	{
		/* Spacing */
		rsvd = 10;
		if (disp_spectrum)			rsvd += 10;
		if (disp_waterfall)			rsvd += 10;
		if (render->options & FRO_LABEL_FREQ)	rsvd += 10;

		/* Divisions */
		if (disp_waterfall) {
			avail = (int)((float)(render->height - rsvd) * render->histo_wf_ratio);
			div   = avail / 10;
			over  = 0;
		} else {
			avail = render->height - rsvd;
			div   = avail / 10;
			over  = avail - (10 * div);
		}

		render->_y_histo_div = (float)div;
		render->_y_histo[1]  = y_top - (float)(over / 2);
		render->_y_histo[0]  = render->_y_histo[1] - (10.0f * render->_y_histo_div) - 1.0f;

		y_top = render->_y_histo[0] - (float)(over / 2) - 10.0f;
	} else {
		render->_y_histo_div = 0.0f;
		render->_y_histo[1]  = 0.0f;
		render->_y_histo[0]  = 0.0f;
	}

	if (render->options & FRO_LABEL_FREQ) {
		if (render->options & FRO_HISTO) {
			render->_y_label = y_top;
			y_top -= 10.0f;
		} else {
			render->_y_label = y_bot;
			y_bot += 10.0f;
		}
	} else {
		render->_y_label = 0.0f;
	}

	if (disp_waterfall) {
		render->_y_wf[1] = y_top;
		render->_y_wf[0] = y_bot;
	} else {
		render->_y_wf[1] = 0.0f;
		render->_y_wf[0] = 0.0f;
	}
}


/*
 * Mapping notes:
 *  - We consider the int x/y coordinates to be the center of that pixels
 *    (so we add 0.5f to each to get the OpenGL coordinates)
 *
 *  - For frequency the OpenGL surface boundaries (so the 'thin' lines between
 *    pixels) are defined as (center - 0.5 * span) -> (center + 0.5 * span)
 *
 *  - For power the center of the drawn line maps to the annotated power level
 *
 *  - For samples we map the first pixel line to sample 0 and the last pixel
 *    line to fft_size * n_spectra. (That's approxiate but pretty much the
 *    best we can do)
 */

double
fosphor_pos2freq(struct fosphor *self, struct fosphor_render *render, int x)
{
	float xf = (float)x + 0.5f;
	float xs = render->_x[1] - render->_x[0];
	float xr = (xf - render->_x[0]) / xs;

	double view_center = self->frequency.center + self->frequency.span * (double)(render->freq_center - 0.5f);
	double view_span   = self->frequency.span * (double)render->freq_span;

	return view_center + view_span * (double)(xr - 0.5f);
}

float
fosphor_pos2pwr(struct fosphor *self, struct fosphor_render *render, int y)
{
	float yf = (float)y;
	float ys = render->_y_histo[1] - render->_y_histo[0] - 1.0f;
	float yr = (yf - render->_y_histo[0]) / ys;

	return self->power.db_ref - 10.0f * self->power.db_per_div * (1.0f - yr);
}

int
fosphor_pos2samp(struct fosphor *self, struct fosphor_render *render, int y)
{
	float yf = (float)y;
	float ys = render->_y_wf[1] - render->_y_wf[0] - 1.0f;
	float yr = (yf - render->_y_wf[0]) / ys;

	return (int)((1.0f - yr) * (float)(FOSPHOR_FFT_LEN * 1024)) * render->wf_span;
}

int
fosphor_freq2pos(struct fosphor *self, struct fosphor_render *render, double freq)
{
	double view_center = self->frequency.center + self->frequency.span * (double)(render->freq_center - 0.5f);
	double view_span   = self->frequency.span * (double)render->freq_span;

	double fr = ((freq - view_center) / view_span);
	float  xs = render->_x[1] - render->_x[0];

	return (int)roundf(render->_x[0] + (float)(fr + 0.5) * xs - 0.5f);
}

int
fosphor_pwr2pos(struct fosphor *self, struct fosphor_render *render, float pwr)
{
	float pr = (self->power.db_ref - pwr) / (10.0f * self->power.db_per_div);
	float ys = render->_y_histo[1] - render->_y_histo[0] - 1.0f;

	return (int)roundf(render->_y_histo[0] + (1.0f - pr) * ys);
}

int
fosphor_samp2pos(struct fosphor *self, struct fosphor_render *render, int time)
{
	float tf = (float)time;
	float tr = tf / ((float)(FOSPHOR_FFT_LEN * 1024) * render->wf_span);
	float ys = render->_y_wf[1] - render->_y_wf[0] - 1.0f;

	return (int)roundf(render->_y_wf[0] + (1.0f - tr) * ys);
}

int
fosphor_render_pos_inside(struct fosphor_render *render, int x, int y)
{
	int in = 0;
	float fx = (float)x;
	float fy = (float)y;

	/* Check X */
	if ((fx >= render->_x[0]) && (fx < render->_x[1]))
		in |= 1;

	/* Histogram */
	if (render->options & (FRO_HISTO | FRO_LIVE | FRO_MAX_HOLD))
	{
		if ((fy >= render->_y_histo[0]) && (fy < render->_y_histo[1]))
			in |= 2;
	}

	/* Waterfall */
	if (render->options & FRO_WATERFALL)
	{
		if ((fy >= render->_y_wf[0]) && (fy < render->_y_wf[1]))
			in |= 4;
	}

	/* Result */
	return in;
}


/*! @} */
