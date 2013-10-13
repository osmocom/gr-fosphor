/*
 * cl.h
 *
 * OpenCL base routines
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

#ifndef __FOSPHOR_CL_H__
#define __FOSPHOR_CL_H__

/*! \defgroup cl
 *  @{
 */

/*! \file cl.h
 *  \brief OpenCL base routines
 */

struct fosphor_gl_state;
struct fosphor_cl_state;

struct fosphor_cl_state *fosphor_cl_init(struct fosphor_gl_state *gl);
void fosphor_cl_release(struct fosphor_cl_state *cl);

int fosphor_cl_process(struct fosphor_cl_state *cl,
                       void *samples, int len);

int  fosphor_cl_get_waterfall_position(struct fosphor_cl_state *cl);
void fosphor_cl_set_histogram_range(struct fosphor_cl_state *cl,
                                    float scale, float offset);

/*! @} */

#endif /* __FOSPHOR_CL_H__ */
