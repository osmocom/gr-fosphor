/*
 * cl.h
 *
 * OpenCL base routines
 *
 * Copyright (C) 2013-2014 Sylvain Munaut
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

#pragma once

/*! \defgroup cl
 *  @{
 */

/*! \file cl.h
 *  \brief OpenCL base routines
 */

struct fosphor;

int  fosphor_cl_init(struct fosphor *self);
void fosphor_cl_release(struct fosphor *self);

int fosphor_cl_process(struct fosphor *self,
                       void *samples, int len);
int fosphor_cl_finish(struct fosphor *self);

void fosphor_cl_load_fft_window(struct fosphor *self, float *win);
int  fosphor_cl_get_waterfall_position(struct fosphor *self);
void fosphor_cl_set_histogram_range(struct fosphor *self,
                                    float scale, float offset);

/*! @} */
