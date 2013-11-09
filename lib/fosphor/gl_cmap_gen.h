/*
 * gl_cmap_gen.h
 *
 * OpenGL color map generators
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

#ifndef __FOSPHOR_GL_CMAP_GEN_H__
#define __FOSPHOR_GL_CMAP_GEN_H__

/*! \addtogroup gl/cmap
 *  @{
 */

/*! \file gl_cmap_gen.h
 *  \brief OpenGL color map generators
 */

#include <stdint.h>

void fosphor_gl_cmap_histogram(uint32_t *rgba, int N);
void fosphor_gl_cmap_waterfall(uint32_t *rgba, int N);

/*! @} */

#endif /* __FOSPHOR_GL_CMAP_GEN_H__ */
