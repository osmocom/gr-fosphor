/*
 * gl_cmap_gen.h
 *
 * OpenGL color map generators
 *
 * Copyright (C) 2013-2021 Sylvain Munaut
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

/*! \addtogroup gl/cmap
 *  @{
 */

/*! \file gl_cmap_gen.h
 *  \brief OpenGL color map generators
 */

#include <stdint.h>

int fosphor_gl_cmap_histogram(uint32_t *rgba, int N, void *arg);
int fosphor_gl_cmap_waterfall(uint32_t *rgba, int N, void *arg);
int fosphor_gl_cmap_prog(uint32_t *rgba, int N, void *arg);
int fosphor_gl_cmap_png(uint32_t *rgba, int N, void *rsrc_name);

/*! @} */
