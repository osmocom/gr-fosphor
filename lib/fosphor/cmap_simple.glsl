/*
 * cmap_simple.glsl
 *
 * Color mapping shader - Nearest/Bilinear version
 *
 * Copyright (C) 2013-2021 Sylvain Munaut
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Note (to make it clear): for the purpose of this license, any software
 * making use of this shader (or derivative thereof) is considered to be
 * a derivative work (i.e. "a work based on the program").
 */

/* Require GLSL 1.5 compatibility profile to build without warning
 * But we use 1.3 since some cards don't allow 1.5 compatibility but
 * will work with 1.3
 */

/* #version 150 compatibility */
#version 130


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
	float intensity = texture(tex, gl_TexCoord[0].st).x;
	float map = (intensity + range.y) * range.x;
	vec4 color = texture(palette, map);
	out_FragColor = color;
}

/* vim: set syntax=c: */
