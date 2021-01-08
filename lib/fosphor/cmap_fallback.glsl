/*
 * cmap_fallback.glsl
 *
 * Color mapping shader - Fall back GLSL 1.0 compatibility version
 *
 * Copyright (C) 2013-2021 Sylvain Munaut
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Note (to make it clear): for the purpose of this license, any software
 * making use of this shader (or derivative thereof) is considered to be
 * a derivative work (i.e. "a work based on the program").
 */

/* ------------------------------------------------------------------------ */
/* Main fragment shader code                                                */
/* ------------------------------------------------------------------------ */

/* Uniforms */

uniform sampler1D palette;	/* 1D texture with the color to map to */
uniform sampler2D tex;		/* 2D intensity texture */
uniform vec2 range;		/* (scale, offset) vector */


/* Shader main */

void main()
{
	float intensity = texture2D(tex, gl_TexCoord[0].st).x;
	float map = (intensity + range.y) * range.x;
	vec4 color = texture1D(palette, map);
	gl_FragColor = color;
}

/* vim: set syntax=c: */
