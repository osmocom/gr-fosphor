/*
 * cl.h
 *
 * OpenCL base routines
 *
 * Copyright (C) 2013-2021 Sylvain Munaut
 * SPDX-License-Identifier: GPL-3.0-or-later
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
