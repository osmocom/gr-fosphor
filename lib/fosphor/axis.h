/*
 * axis.h
 *
 * Logic to deal with various axises
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

#ifndef __FOSPHOR_AXIS_H__
#define __FOSPHOR_AXIS_H__

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

#endif /* __FOSPHOR_AXIS_H__ */
