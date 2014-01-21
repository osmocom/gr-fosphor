/*
 * fosphor.c
 *
 * Main fosphor entry point
 *
 * Copyright (C) 2013 Sylvain Munaut
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
		FRO_LABEL_TIME;

	render->histo_wf_ratio = 0.5f;
	render->freq_start     = 0.0f;
	render->freq_stop      = 1.0f;
	render->wf_span        = 1.0f;
}

void
fosphor_render_refresh(struct fosphor_render *render)
{
	int disp_spectrum, disp_waterfall;
	int avail, div, rsvd;
	float over, y_top, y_bot;

	/* Which screen zone ? */
	disp_spectrum  = !!(render->options & (FRO_LIVE | FRO_MAX_HOLD | FRO_HISTO));
	disp_waterfall = !!(render->options & FRO_WATERFALL);

	/* Split the X space */
	if (render->options & (FRO_LABEL_PWR | FRO_LABEL_TIME))
		rsvd = 50;
	else
		rsvd = 20;

	avail = render->width - rsvd;
	div   = avail / 10;
	over  = avail - (10 * div);

	render->_x_div = (float)div;
	render->_x[0] = render->pos_x + (float)(rsvd - 10) + (float)(over / 2);
	render->_x[1] = render->_x[0] + (10.0f * render->_x_div) + 1.0f;
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

/*! @} */
