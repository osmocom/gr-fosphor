/*
 * gl_cmap_gen.c
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

/*! \addtogroup gl/cmap
 *  @{
 */

/*! \file gl_cmap_gen.c
 *  \brief OpenGL color map generators
 */

#include <stdint.h>
#include <math.h>

#include "gl_cmap_gen.h"


static void
_hsv2rgb(float *rgb, float *hsv)
{
	int i;
	float h, s, v;
	float r, g, b;
	float f, p, q, t;

	/* Unpack input */
	h = hsv[0];
	s = hsv[1];
	v = hsv[2];

	if( s <= 0.0f ) {
		/* achromatic (grey) */
		r = g = b = v;
		goto done;
	}

	h *= 5.0f;			/* sector 0 to 5 */
	i = floor(h);
	f = h - i;			/* fractional part of h */
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );

	switch (i % 6) {
		case 0:
			r = v;
			g = t;
			b = p;
			break;
		case 1:
			r = q;
			g = v;
			b = p;
			break;
		case 2:
			r = p;
			g = v;
			b = t;
			break;
		case 3:
			r = p;
			g = q;
			b = v;
			break;
		case 4:
			r = t;
			g = p;
			b = v;
			break;
		case 5:
		default:
			r = v;
			g = p;
			b = q;
			break;
	}

done:
	/* Pack results */
	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}


static inline void
_set_rgba_from_hsv(uint32_t *rgba, float h, float s, float v)
{
		float hsv[3], rgb[3];
		unsigned char r,g,b,a;

		hsv[0] = h;
		hsv[1] = s;
		hsv[2] = v;

		_hsv2rgb(rgb, hsv);

		r = (unsigned char) round(rgb[0] * 255.0f);
		g = (unsigned char) round(rgb[1] * 255.0f);
		b = (unsigned char) round(rgb[2] * 255.0f);
		a = 255;

		*rgba = (a << 24) | (b << 16) | (g << 8) | r;
}


void
fosphor_gl_cmap_histogram(uint32_t *rgba, int N)
{
	int i;
	int m = N >> 4;

	for (i=0; i<m; i++)
	{
		float p = (1.0f * i) / (N - 1);

		_set_rgba_from_hsv(&rgba[i],
			0.90f,				/* H */
			0.50f,				/* S */
			0.15f + 4.0f * p		/* V */
		);
	}

	for (i=m; i<N; i++)
	{
		float p = (1.0f * i) / (N - 1);

		_set_rgba_from_hsv(&rgba[i],
			0.80f -   p * 0.80f,
			1.00f - ((p < 0.85f) ? 0.0f : ((p - 0.85f) * 3.0f)),
			0.60f + ((p < 0.40f) ? p : 0.40f)
		);
	}
}


void
fosphor_gl_cmap_waterfall(uint32_t *rgba, int N)
{
	int i;

	for (i=0; i<N; i++)
	{
		float p = (1.0f * i) / (N - 1);

		_set_rgba_from_hsv(&rgba[i],
			0.75f - (p * 0.75f),	/* H */
			1.0f,			/* S */
			((p * 0.95f) + 0.05f)	/* V */
		);
	}
}

/*! @} */
