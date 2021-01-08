/*
 * gl.h
 *
 * OpenGL part of fosphor
 *
 * Copyright (C) 2013-2021 Sylvain Munaut
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

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

void fosphor_gl_refresh(struct fosphor *self);
void fosphor_gl_draw(struct fosphor *self, struct fosphor_render *render);

/*! @} */
