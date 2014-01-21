/*
 * gl.h
 *
 * OpenGL part of fosphor
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

#ifndef __FOSPHOR_GL_H__
#define __FOSPHOR_GL_H__

/*! \defgroup gl
 *  @{
 */

/*! \file gl.h
 *  \brief OpenGL part of fosphor
 */

#include "gl_platform.h"

struct fosphor;
struct fosphor_render;

int  fosphor_gl_init(struct fosphor *self);
void fosphor_gl_release(struct fosphor *self);


enum fosphor_gl_id {
	GL_ID_TEX_WATERFALL,
	GL_ID_TEX_HISTOGRAM,
	GL_ID_VBO_SPECTRUM,
};

GLuint fosphor_gl_get_shared_id(struct fosphor *self,
                                enum fosphor_gl_id id);

void fosphor_gl_draw(struct fosphor *self, struct fosphor_render *render);

/*! @} */

#endif /* __FOSPHOR_GL_H__ */
