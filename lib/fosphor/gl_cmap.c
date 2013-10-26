/*
 * gl_cmap.c
 *
 * OpenGL float texture -> color mapping
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

/*! \file gl_cmap.c
 *  \brief OpenGL float texture to color mapping
 */

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "gl_platform.h"

#include "gl_cmap.h"
#include "resource.h"


struct gl_cmap_shader
{
	/* Status */
	int loaded;

	/* Shader handles */
	GLuint prog;
	GLuint shader;

	/* Uniforms */
	GLint u_tex;
	GLint u_palette;
	GLint u_range;
};

enum gl_cmap_shader_type
{
	GL_CMAP_SHADER_SIMPLE,
	GL_CMAP_SHADER_BICUBIC,
	GL_CMAP_SHADER_FALLBACK,
	_GL_CMAP_SHADER_NUM
};

struct fosphor_gl_cmap_ctx {
	struct gl_cmap_shader shaders[_GL_CMAP_SHADER_NUM];
};


/* -------------------------------------------------------------------------- */
/* Helpers / Internal API                                                     */
/* -------------------------------------------------------------------------- */

static int
gl_cmap_init_shader(struct gl_cmap_shader *shader, const char *name)
{
	const char *shader_src;
	GLint buf_len, orv;

	/* Load shader sources */
	shader_src = resource_get(name, NULL);
	if (!shader_src)
		return -ENOENT;

	/* Allocate shader */
	shader->prog = glCreateProgram();
	shader->shader = glCreateShader(GL_FRAGMENT_SHADER);

	/* Compile / Link / Attach */
	glShaderSource(shader->shader, 1, (const char **)&shader_src, NULL);
	glCompileShader(shader->shader);

	/* Check success and compile log */
	glGetShaderiv(shader->shader, GL_COMPILE_STATUS, &orv);
	glGetShaderiv(shader->shader, GL_INFO_LOG_LENGTH, &buf_len);

#if 1
	if ((buf_len > 0) && (orv != GL_TRUE))
#else
	if (buf_len > 0)
#endif
	{
		char *buf = malloc(buf_len+1);

		glGetShaderInfoLog(shader->shader, buf_len, 0, buf);
		buf[buf_len] = '\0';

		fprintf(stderr, "[!] gl_cmap shader compile log :\n%s\n", buf);

		free(buf);
	}

	if (orv != GL_TRUE) {
		fprintf(stderr, "[!] gl_cmap shader compilation failed (%s)\n", name);
		return -EINVAL;
	}

	/* Attach to program */
	glAttachShader(shader->prog, shader->shader);
	glLinkProgram(shader->prog);

	/* Grab the uniform locations */
	shader->u_tex     = glGetUniformLocation(shader->prog, "tex");
	shader->u_palette = glGetUniformLocation(shader->prog, "palette");
	shader->u_range   = glGetUniformLocation(shader->prog, "range");

	/* Success */
	shader->loaded = 1;

	return 0;
}

static void
gl_cmap_release_shader(struct gl_cmap_shader *shader)
{
	if (!shader->loaded)
		return;

	glDetachShader(shader->prog, shader->shader);
	glDeleteShader(shader->shader);
	glDeleteProgram(shader->prog);

	memset(shader, 0x00, sizeof(struct gl_cmap_shader));
}


/* -------------------------------------------------------------------------- */
/* Exposed API                                                                */
/* -------------------------------------------------------------------------- */

struct fosphor_gl_cmap_ctx *
fosphor_gl_cmap_init(void)
{
	struct fosphor_gl_cmap_ctx *cmap_ctx;
	int rv;
	int need_fallback = 0;

	/* Allocate structure */
	cmap_ctx = malloc(sizeof(struct fosphor_gl_cmap_ctx));
	if (!cmap_ctx)
		return NULL;

	memset(cmap_ctx, 0, sizeof(struct fosphor_gl_cmap_ctx));

	/* Init shaders */
	rv = gl_cmap_init_shader(
		&cmap_ctx->shaders[GL_CMAP_SHADER_SIMPLE],
		"cmap_simple.glsl"
	);
	if (rv) {
		fprintf(stderr, "[w] Color map shader 'simple' failed to load, will use fallback\n");
		need_fallback = 1;
	}

	rv = gl_cmap_init_shader(
		&cmap_ctx->shaders[GL_CMAP_SHADER_BICUBIC],
		"cmap_bicubic.glsl"
	);
	if (rv) {
		fprintf(stderr, "[w] Color map shader 'bicubic' failed to load, will use fallback\n");
		need_fallback = 1;
	}

	if (need_fallback) {
		rv = gl_cmap_init_shader(
			&cmap_ctx->shaders[GL_CMAP_SHADER_FALLBACK],
			"cmap_fallback.glsl"
		);
		if (rv) {
			fprintf(stderr, "[!] Color map shader 'fallback' failed, aborting\n");
			goto error;
		}
	}

	/* All done */
	return cmap_ctx;

	/* Error path */
error:
	fosphor_gl_cmap_release(cmap_ctx);

	return NULL;
}

void
fosphor_gl_cmap_release(struct fosphor_gl_cmap_ctx *cmap_ctx)
{
	/* Safety */
	if (!cmap_ctx)
		return;

	/* Disable program */
	glUseProgram(0);

	/* Release shaders */
	gl_cmap_release_shader(&cmap_ctx->shaders[GL_CMAP_SHADER_SIMPLE]);
	gl_cmap_release_shader(&cmap_ctx->shaders[GL_CMAP_SHADER_BICUBIC]);
	gl_cmap_release_shader(&cmap_ctx->shaders[GL_CMAP_SHADER_FALLBACK]);
}


void
fosphor_gl_cmap_enable(struct fosphor_gl_cmap_ctx *cmap_ctx,
                       GLuint tex_id, GLuint cmap_id,
                       float scale, float offset,
                       enum fosphor_gl_cmap_mode mode)
{
	struct gl_cmap_shader *shader;
	float range[2];
	int fmode;

	shader = &cmap_ctx->shaders[
		mode == GL_CMAP_MODE_BICUBIC ?
			GL_CMAP_SHADER_BICUBIC :
			GL_CMAP_SHADER_SIMPLE
	];

	if (!shader->loaded)
		shader = &cmap_ctx->shaders[GL_CMAP_SHADER_FALLBACK];

	/* Enable program */
	glUseProgram(shader->prog);

	/* Texture unit 0: Main texture */
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_id);
	glUniform1i(shader->u_tex, 0);

	fmode = (mode == GL_CMAP_MODE_NEAREST) ? GL_NEAREST : GL_LINEAR;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, fmode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, fmode);

	/* Texture unit 1: Palette 1D texture */
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_1D, cmap_id);
	glUniform1i(shader->u_palette, 1);

	/* Range definition */
	range[0] = scale;
	range[1] = offset;
	glUniform2fv(shader->u_range, 1, range);
}

void
fosphor_gl_cmap_disable(void)
{
	/* Default to texture unit 0 */
	glActiveTexture(GL_TEXTURE0);

	/* Disable program */
	glUseProgram(0);
}


int
fosphor_gl_cmap_generate(GLuint *cmap_id, gl_cmap_gen_func_t gfn, int N)
{
	uint32_t *rgba;

	/* Temp array for data */
	rgba = malloc(sizeof(uint32_t) * N);
	if (!rgba)
		return -1;

	/* Allocate texture ID */
	glGenTextures(1, cmap_id);

	/* Select it */
	glBindTexture(GL_TEXTURE_1D, *cmap_id);

	/* Generate texture data */
	gfn(rgba, N);

	/* Upload data */
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, N, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, rgba);

	/* Configure texture behavior */
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

	/* Release temp array */
	free(rgba);

	return 0;
}

/*! @} */
