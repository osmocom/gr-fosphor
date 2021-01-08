/*
 * gl_cmap.h
 *
 * OpenGL float texture -> color mapping
 *
 * Copyright (C) 2013-2021 Sylvain Munaut
 * SPDX-License-Identifier: GPL-3.0-or-later
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
