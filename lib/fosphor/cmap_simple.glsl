/*
 * cmap_simple.glsl
 *
 * Color mapping shader - Nearest/Bilinear version
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
