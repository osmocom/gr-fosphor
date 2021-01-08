/*
 * gl_cmap.h
 *
 * OpenGL float texture -> color mapping
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

/*! \defgroup gl/cmap
 *  @{
 */

/*! \file gl_cmap.h
 *  \brief OpenGL float texture to color mapping
 */

#include <stdint.h>

#include "gl_platform.h"


struct fosphor_gl_cmap_ctx;

enum fosphor_gl_cmap_mode {
	GL_CMAP_MODE_NEAREST,
	GL_CMAP_MODE_BILINEAR,
	GL_CMAP_MODE_BICUBIC,
};


struct fosphor_gl_cmap_ctx *fosphor_gl_cmap_init(void);
void fosphor_gl_cmap_release(struct fosphor_gl_cmap_ctx *cmap_ctx);

void fosphor_gl_cmap_enable(struct fosphor_gl_cmap_ctx *cmap_ctx,
                            GLuint tex_id, GLuint cmap_id,
                            float scale, float offset,
                            enum fosphor_gl_cmap_mode mode);
void fosphor_gl_cmap_disable(void);

void fosphor_gl_cmap_draw_scale(GLuint cmap_id,
                                float x0, float x1, float y0, float y1);

typedef int (*gl_cmap_gen_func_t)(uint32_t *rgba, int N, void *arg);
int fosphor_gl_cmap_generate(GLuint *cmap_id, gl_cmap_gen_func_t gfn, void *gfn_arg, int N);


/*! @} */
