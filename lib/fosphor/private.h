/*
 * private.h
 *
 * Private fosphor definitions
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

#ifndef __FOSPHOR_PRIVATE_H__
#define __FOSPHOR_PRIVATE_H__

/*! \defgroup private
 *  @{
 */

/*! \file private.h
 *  \brief Private fosphor definitions
 */


#define FOSPHOR_FFT_LEN_LOG	10
#define FOSPHOR_FFT_LEN		(1<<FOSPHOR_FFT_LEN_LOG)

#define FOSPHOR_FFT_MULT_BATCH	16
#define FOSPHOR_FFT_MAX_BATCH	1024

struct fosphor_cl_state;
struct fosphor_gl_state;

struct fosphor
{
	struct fosphor_cl_state *cl;
	struct fosphor_gl_state *gl;

	struct {
		int db_ref;
		int db_per_div;
		float scale;
		float offset;
	} power;

	struct {
		double center;
		double span;
	} frequency;
};


/*! @} */

#endif /* __FOSPHOR_PRIVATE_H__ */
