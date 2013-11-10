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
fosphor_draw(struct fosphor *self, int w, int h)
{
	int wf_pos = fosphor_cl_get_waterfall_position(self);
	fosphor_gl_draw(self, w, h, wf_pos);
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

/*! @} */
