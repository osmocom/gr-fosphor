/*
 * private.h
 *
 * Private fosphor definitions
 *
 * Copyright (C) 2013-2021 Sylvain Munaut
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

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

#define FLG_FOSPHOR_USE_CLGL_SHARING	(1<<0)
	int flags;

	float fft_win[FOSPHOR_FFT_LEN];

	float *img_waterfall;
	float *img_histogram;
	float *buf_spectrum;

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
