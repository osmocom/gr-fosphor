/*
 * gl_cmap_gen.c
 *
 * OpenGL color map generators
 *
 * Copyright (C) 2013-2021 Sylvain Munaut
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/*! \addtogroup gl/cmap
 *  @{
 */

/*! \file gl_cmap_gen.c
 *  \brief OpenGL color map generators
 */

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "gl_cmap_gen.h"
#include "resource.h"


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

static void
_set_rgba_from_rgb(uint32_t *rgba, float r, float g, float b)
{
	unsigned char rc,gc,bc,ac;

	rc = (unsigned char) roundf(r * 255.0f);
	gc = (unsigned char) roundf(g * 255.0f);
	bc = (unsigned char) roundf(b * 255.0f);
	ac = 255;

	*rgba = (ac << 24) | (bc << 16) | (gc << 8) | rc;
}

static void
_set_rgba_from_hsv(uint32_t *rgba, float h, float s, float v)
{
	float hsv[3], rgb[3];

	hsv[0] = h;
	hsv[1] = s;
	hsv[2] = v;

	_hsv2rgb(rgb, hsv);

	_set_rgba_from_rgb(rgba, rgb[0], rgb[1], rgb[2]);
}


static inline uint32_t
_rgba_interpolate(uint32_t *rgba, int sz, int p, int N)
{
	int pos_i = (p * (sz-1)) / (N-1);
	int pos_f = (p * (sz-1)) - (pos_i * (N-1));
	uint32_t vl, vh, vf = 0;
	int i;

	if (pos_f == 0)
		return rgba[pos_i];

	vl = rgba[pos_i];
	vh = rgba[pos_i+1];

	for (i=0; i<4; i++)
	{
		uint32_t nv =
			((vl >> (8 * i)) & 0xff) * ((N-1) - pos_f) +
			((vh >> (8 * i)) & 0xff) * pos_f;

		vf |= ((nv / (N-1)) & 0xff) << (8 * i);
	}

	return vf;
}


int
fosphor_gl_cmap_histogram(uint32_t *rgba, int N, void *arg)
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

	return 0;
}


int
fosphor_gl_cmap_waterfall(uint32_t *rgba, int N, void *arg)
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

	return 0;
}


#ifdef ENABLE_PNG
#include <png.h>
int
fosphor_gl_cmap_png(uint32_t *rgba, int N, void *arg)
{
	const char *rsrc_name = arg;
	png_image img;
	const void *png_data = NULL;
	void *png_rgba = NULL;
	int png_len, i, rv;

	/* Grab the file */
	png_data = resource_get(rsrc_name, &png_len);
	if (!png_data)
		return -ENOENT;

	/* Read PNG */
	memset(&img, 0x00, sizeof(img));
	img.version = PNG_IMAGE_VERSION;

	rv = png_image_begin_read_from_memory(&img, png_data, png_len);
	if (!rv) {
		rv = -EINVAL;
		goto error;
	}

	img.format = PNG_FORMAT_RGBA;

	png_rgba = malloc(sizeof(uint32_t) * img.width * img.height);
	if (!png_rgba) {
		rv = -ENOMEM;
		goto error;
	}

	rv = png_image_finish_read(&img,
		NULL,				/* background */
		png_rgba,			/* buffer */
		sizeof(uint32_t) * img.width,	/* row_stride */
		NULL				/* colormap */
	);
	if (!rv) {
		rv = -EINVAL;
		goto error;
	}

	/* Interpolate the PNG to the requested linear scale */
	for (i=0; i<N; i++)
		rgba[i] = _rgba_interpolate(png_rgba, img.width, i, N);

	/* Done */
	rv = 0;

error:
	free(png_rgba);

	if (png_data)
		resource_put(png_data);

	return rv;
}
#else
int
fosphor_gl_cmap_png(uint32_t *rgba, int N, void *arg)
{
	return -ENOTSUP;
}
#endif


int
fosphor_gl_cmap_prog(uint32_t *rgba, int N, void *arg)
{
	const struct {
		float p;
		float rgb[3];
	} colors[] = {
		{ .p =  0.0f, .rgb = { 0.29f, 0.00f, 0.00f } },
		{ .p =  1.0f, .rgb = { 0.46f, 0.00f, 0.00f } },
		{ .p =  2.0f, .rgb = { 0.62f, 0.00f, 0.00f } },
		{ .p =  3.0f, .rgb = { 0.78f, 0.00f, 0.00f } },
		{ .p =  4.0f, .rgb = { 1.00f, 0.00f, 0.00f } },
		{ .p =  5.0f, .rgb = { 1.00f, 0.43f, 0.10f } },
		{ .p =  6.0f, .rgb = { 1.00f, 1.00f, 0.00f } },
		{ .p =  7.0f, .rgb = { 1.00f, 1.00f, 1.00f } },
		{ .p =  8.0f, .rgb = { 0.11f, 0.56f, 1.00f } },
		{ .p =  9.0f, .rgb = { 0.00f, 0.00f, 0.57f } },
		{ .p = 10.0f, .rgb = { 0.00f, 0.00f, 0.31f } },
		{ .p = 11.0f, .rgb = { 0.00f, 0.00f, 0.19f } },
		{ .p = 12.0f, .rgb = { 0.00f, 0.00f, 0.12f } },
	};

	const int NC = 12;
	int i;

	for (i=0; i<N; i++)
	{
		float rgb[3];
		float p = 1.0f - ((1.0f * i) / (N - 1));
		float ps = p * 9.0f;
		float m;
		int li, j;

		/* Scan for li */
		for (li=0; li<(NC-1) && colors[li+1].p < ps; li++);

		/* Linear interpolation */
		ps -= colors[li].p;
		ps /= colors[li+1].p - colors[li].p;

		m = ps;

		/* Mix */
		for (j=0; j<3; j++)
			rgb[j] = colors[li].rgb[j] * (1.0f - m) + colors[li+1].rgb[j] * m;

		/* Set resulting color */
		_set_rgba_from_rgb(&rgba[i], rgb[0], rgb[1], rgb[2]);
	}

	return 0;
}

/*! @} */
