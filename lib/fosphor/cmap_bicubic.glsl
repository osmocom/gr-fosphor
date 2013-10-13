/*
 * cmap_bicubic.glsl
 *
 * Color mapping shader - BiCubic interpolation
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
 *
 * Note (to make it clear): for the purpose of this license, any software
 * making use of this shader (or derivative thereof) is considered to be
 * a derivative work (i.e. "a work based on the program").
 */

/* Require GLSL 1.5 compatibility profile */
#version 150 compatibility


/* ------------------------------------------------------------------------ */
/* Cubic interpolation functions                                            */
/* ------------------------------------------------------------------------ */

/* Triangular */
float if_triangular(float x)
{
	x /= 2.0;
	return (x < 0.0) ? (x + 1.0) : (1.0 - x);
}

/* Bell curve */
float if_bell(float x)
{
	x *= 0.75; /* [-2:2] -> [-1.5:1.5] */

	if ((x > -1.5) && (x < -0.5))
		return 0.5 * pow(x + 1.5, 2.0);

	else if ((x > -0.5) && (x < 0.5))
		return 3.0 / 4.0 - (x * x);

	else if ((x > 0.5) && (x < 1.5))
		return 0.5 * pow(x - 1.5, 2.0);

	return 0.0;
}

/* B-Spline */
float if_bspline(float x)
{
	if (x < 0.0)
		x = -x;

	if ((x >= 0.0) && (x <= 1.0))
		return (2.0 / 3.0) + 0.5 * (x * x * x) - (x*x);

	else if( x > 1.0 && x <= 2.0 )
		return (1.0 / 6.0) * pow((2.0 - x), 3.0);

	return 1.0;
}

/* Catmull-Rom Spline */
float if_catmull_rom(float x)
{
	const float B = 0.0;
	const float C = 0.5;

	if (x < 0.0)
		x = -x;

	if (x < 1.0)
		return (
			( 12.0 -  9.0 * B - 6.0 * C) * ( x * x * x ) +
			(-18.0 + 12.0 * B + 6.0 * C) * ( x * x ) +
			(  6.0 -  2.0 * B )
		) / 6.0;

	else if ((x >= 1.0) && (x < 2.0))
		return (
			( -       B -  6.0 * C ) * ( x * x * x ) +
			(   6.0 * B + 30.0 * C ) * ( x *x ) +
			( -12.0 * B - 48.0 * C ) *   x +
			(   8.0 * B + 24.0 * C )
		) / 6.0;

	return 0.0;
}


/* ------------------------------------------------------------------------ */
/* Bi-Cubic texture sampling                                                */
/* ------------------------------------------------------------------------ */

/* Select the interpolation function */
#ifndef if_func
#define if_func if_bspline
#endif

/* Returns the weight for the 4 pixels in the cubic interpolation */
vec4 cubic(float x)
{
	return vec4(
		if_func(-1.0 - x),
		if_func(     - x),
		if_func( 1.0 - x),
		if_func( 2.0 - x)
	);
}

/* bicubic sampler */
vec4 bicubic(sampler2D tex_id, vec2 tex_coord)
{
	ivec2 tex_size  = textureSize(tex_id, 0);
	vec2  tex_scale = 1.0 / vec2(tex_size);

	tex_coord *= tex_size;

	float fx = fract(tex_coord.x);
	float fy = fract(tex_coord.y);
	tex_coord.x -= fx;
	tex_coord.y -= fy;

	vec4 xcubic = cubic(fx);
	vec4 ycubic = cubic(fy);

	vec4 c = vec4(
		tex_coord.x - 0.5,
		tex_coord.x + 1.5,
		tex_coord.y - 0.5,
		tex_coord.y + 1.5
	);
	vec4 s = vec4(
		xcubic.x + xcubic.y,
		xcubic.z + xcubic.w,
		ycubic.x + ycubic.y,
		ycubic.z + ycubic.w
	);
	vec4 offset = c + vec4(xcubic.y, xcubic.w, ycubic.y, ycubic.w) / s;

	vec4 s0 = texture(tex_id, offset.xz * tex_scale);
	vec4 s1 = texture(tex_id, offset.yz * tex_scale);
	vec4 s2 = texture(tex_id, offset.xw * tex_scale);
	vec4 s3 = texture(tex_id, offset.yw * tex_scale);

	float sx = s.x / (s.x + s.y);
	float sy = s.z / (s.z + s.w);

	return mix(
		mix(s3, s2, sx),
		mix(s1, s0, sx),
		sy
	);
}


/* ------------------------------------------------------------------------ */
/* Main fragment shader code                                                */
/* ------------------------------------------------------------------------ */

/* Uniforms */

uniform sampler1D palette;	/* 1D texture with the color to map to */
uniform sampler2D tex;		/* 2D intensity texture */
uniform vec2 range;		/* (scale, offset) vector */


/* In/Out */

out vec4 out_FragColor;


/* Shader main */

void main()
{
	float intensity = bicubic(tex, gl_TexCoord[0].st).x;
	float map = (intensity + range.y) * range.x;
	vec4 color = texture(palette, map);
	out_FragColor = color;
}

/* vim: set syntax=c: */
