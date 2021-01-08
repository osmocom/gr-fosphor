/*
 * axis.h
 *
 * Logic to deal with various axises
 *
 * Copyright (C) 2013-2021 Sylvain Munaut
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

/*! \defgroup axis
 *  @{
 */

/*! \file axis.h
 *  \brief Logic to deal with various axises
 */

struct freq_axis
{
	double center;
	double span;
	double step;
	int    mode;
	char   abs_fmt[16];
	double abs_scale;
	char   rel_fmt[16];
	double rel_step;
};

void freq_axis_build(struct freq_axis *fx, double center, double span, int n_div);
void freq_axis_render(struct freq_axis *fx, char *str, int step);

/*! @} */
