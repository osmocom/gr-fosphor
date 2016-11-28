/*
 * gl.c
 *
 * OpenGL part of fosphor
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

/*! \addtogroup gl
 *  @{
 */

/*! \file gl.c
 *  \brief OpenGL part of fosphor
 */

#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "gl_platform.h"

#include "axis.h"
#include "fosphor.h"
#include "gl.h"
#include "gl_cmap.h"
#include "gl_cmap_gen.h"
#include "gl_font.h"
#include "private.h"
#include "resource.h"


struct fosphor_gl_state
{
	int init_complete;

	struct gl_font *font;

	struct fosphor_gl_cmap_ctx *cmap_ctx;
	GLuint cmap_waterfall;
	GLuint cmap_histogram;

	GLuint tex_waterfall;
	GLuint tex_histogram;

	GLuint vbo_spectrum;
};


/* -------------------------------------------------------------------------- */
/* Helpers / Internal API                                                     */
/* -------------------------------------------------------------------------- */

static int
gl_check_extension(const char *ext_name)
{
	const char *ext_str;
	const char *p;
	int l = strlen(ext_name);

	ext_str = (const char *)glGetString(GL_EXTENSIONS);
	if (!ext_str) {
		fprintf(stderr, "[w] Failed to retrieve GL extension list.\n");
		return 0;
	}

	for (p=ext_str; (p=strstr(p, ext_name)) != NULL; p++)
	{
		if ((p != ext_str) && (p[-1] != ' '))
			continue;
		if ((p[l] != 0x00) && (p[l] != ' '))
			continue;
		return 1;
	}

	return 0;
}

static void
gl_tex2d_float_clear(GLuint tex_id, int width, int height)
{
	float buf[16*16];
	int x, y, cw, ch;

	memset(buf, 0x00, sizeof(buf));

	glBindTexture(GL_TEXTURE_2D, tex_id);

	for (y=0; y<height; y+=16) {
		for (x=0; x<width; x+=16) {
			cw = ((x+16) > width ) ? (width  - x) : 16;
			ch = ((y+16) > height) ? (height - y) : 16;
			glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, cw, ch, GL_RED, GL_FLOAT, buf);
		}
	}
}

static void
gl_tex2d_write(GLuint tex_id, float *src, int width, int height)
{
	glBindTexture(GL_TEXTURE_2D, tex_id);

	glTexSubImage2D(
		GL_TEXTURE_2D, 0,
		0, 0, width, height,
		GL_RED, GL_FLOAT,
		src
	);
}

static void
gl_vbo_clear(GLuint vbo_id, int size)
{
	void *ptr;

	glBindBuffer(GL_ARRAY_BUFFER, vbo_id);

	ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	if (!ptr)
		abort();

	memset(ptr, 0x00, size);

	glUnmapBuffer(GL_ARRAY_BUFFER);
}

#if 0
static void
gl_vbo_read(GLuint vbo_id, void *dst, int size)
{
	void *ptr;

	glBindBuffer(GL_ARRAY_BUFFER, vbo_id);

	ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	if (!ptr)
		abort();

	memcpy(dst, ptr, size);

	glUnmapBuffer(GL_ARRAY_BUFFER);
}
#endif

static void
gl_vbo_write(GLuint vbo_id, void *src, int size)
{
#if 0
	void *ptr;

	glBindBuffer(GL_ARRAY_BUFFER, vbo_id);

	ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	if (!ptr)
		abort();

	memcpy(ptr, src, size);

	glUnmapBuffer(GL_ARRAY_BUFFER);
#else
	glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
	glBufferData(GL_ARRAY_BUFFER, size, src, GL_DYNAMIC_DRAW);
#endif
}

static void
gl_deferred_init(struct fosphor *self)
{
	struct fosphor_gl_state *gl = self->gl;
	GLint tex_fmt;
	int len;

	/* Prevent double init */
	if (gl->init_complete)
		return;

	gl->init_complete = 1;

	/* Select texture format */
	tex_fmt = gl_check_extension("GL_ARB_texture_rg") ?
		GL_R32F :
		GL_LUMINANCE32F_ARB;

	/* Waterfall texture (FFT_LEN * 1024) */
	glGenTextures(1, &gl->tex_waterfall);

	glBindTexture(GL_TEXTURE_2D, gl->tex_waterfall);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, tex_fmt, FOSPHOR_FFT_LEN, 1024, 0, GL_RED, GL_FLOAT, NULL);

	gl_tex2d_float_clear(gl->tex_waterfall, FOSPHOR_FFT_LEN, 1024);

	/* Histogram texture (FFT_LEN * 128) */
	glGenTextures(1, &gl->tex_histogram);

	glBindTexture(GL_TEXTURE_2D, gl->tex_histogram);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, tex_fmt, FOSPHOR_FFT_LEN, 128, 0, GL_RED, GL_FLOAT, NULL);

	gl_tex2d_float_clear(gl->tex_histogram, FOSPHOR_FFT_LEN, 128);

	/* Spectrum VBO (2 * FFT_LEN, half for live, half for 'hold') */
	glGenBuffers(1, &gl->vbo_spectrum);

	glBindBuffer(GL_ARRAY_BUFFER, gl->vbo_spectrum);

	len = 2 * sizeof(float) * 2 * FOSPHOR_FFT_LEN;
	glBufferData(GL_ARRAY_BUFFER, len, NULL, GL_DYNAMIC_DRAW);

	gl_vbo_clear(gl->vbo_spectrum, len);
}


/* -------------------------------------------------------------------------- */
/* Exposed API                                                                */
/* -------------------------------------------------------------------------- */

int
fosphor_gl_init(struct fosphor *self)
{
	struct fosphor_gl_state *gl;
	const void *font_data;
	int len, rv;

	/* Allocate structure */
	gl = malloc(sizeof(struct fosphor_gl_state));
	if (!gl)
		return -ENOMEM;

	self->gl = gl;

#ifdef USING_GLEW
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
	  /* Problem: glewInit failed, something is seriously wrong. */
	  fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
#endif
	
	memset(gl, 0, sizeof(struct fosphor_gl_state));

	/* Font */
	gl->font = glf_alloc(8, GLF_FLG_LCD);
	if (!gl->font) {
		rv = -ENOMEM;
		goto error;
	}

	font_data = resource_get("DroidSansMonoDotted.ttf", &len);
	if (!font_data) {
		rv = -ENOENT;
		goto error;
	}

	rv = glf_load_face_mem(gl->font, font_data, len);
	if (rv)
		goto error;

	/* Color mapping */
	gl->cmap_ctx = fosphor_gl_cmap_init();

	rv  = (gl->cmap_ctx == NULL);

	rv |= fosphor_gl_cmap_generate(&gl->cmap_waterfall,
	                               fosphor_gl_cmap_waterfall, NULL, 256);
	rv |= fosphor_gl_cmap_generate(&gl->cmap_histogram,
	                               fosphor_gl_cmap_histogram, NULL, 256);

	if (rv)
		goto error;

	/* Done */
	return 0;

error:
	fosphor_gl_release(self);

	return rv;
}

void
fosphor_gl_release(struct fosphor *self)
{
	struct fosphor_gl_state *gl = self->gl;

	/* Safety */
	if (!gl)
		return;

	/* Release all */
	glDeleteBuffers(1, &gl->vbo_spectrum);

	glDeleteTextures(1, &gl->tex_histogram);
	glDeleteTextures(1, &gl->tex_waterfall);

	glDeleteTextures(1, &gl->cmap_histogram);
	glDeleteTextures(1, &gl->cmap_waterfall);
	fosphor_gl_cmap_release(gl->cmap_ctx);

	glf_free(gl->font);

	/* Release structure */
	free(gl);

	/* Nothing left */
	self->gl = NULL;
}


GLuint
fosphor_gl_get_shared_id(struct fosphor *self,
                         enum fosphor_gl_id id)
{
	struct fosphor_gl_state *gl = self->gl;

	/* CL is now sufficiently booted to complete the GL init
	 * in a CL context */
	gl_deferred_init(self);

	/* Select ID to return */
	switch (id) {
	case GL_ID_TEX_WATERFALL:
		return gl->tex_waterfall;

	case GL_ID_TEX_HISTOGRAM:
		return gl->tex_histogram;

	case GL_ID_VBO_SPECTRUM:
		return gl->vbo_spectrum;
	}

	return 0;
}


void
fosphor_gl_refresh(struct fosphor *self)
{
	struct fosphor_gl_state *gl = self->gl;

	if (self->flags & FLG_FOSPHOR_USE_CLGL_SHARING)
		return;

	gl_deferred_init(self);

	gl_tex2d_write(gl->tex_waterfall, self->img_waterfall, FOSPHOR_FFT_LEN, 1024);
	gl_tex2d_write(gl->tex_histogram, self->img_histogram, FOSPHOR_FFT_LEN,  128);
	gl_vbo_write(gl->vbo_spectrum, self->buf_spectrum, 2 * 2 * sizeof(float) * FOSPHOR_FFT_LEN);
}


void
fosphor_gl_draw(struct fosphor *self, struct fosphor_render *render)
{
	struct fosphor_gl_state *gl = self->gl;
	struct freq_axis freq_axis;
	float x[2], y[2], u[2], v[2];
	float tw, bw;
	int i;

	/* Utils */
	tw = 1.0f / (float)FOSPHOR_FFT_LEN;	/* Texel width */
	bw = 1.0f / (float)(FOSPHOR_FFT_LEN-1);	/* Bin width (displayed) */

	/* Texture mapping notes:
	 *
	 *  - The texture have the "DC" bin at texel 0, however we want it to
	 *    be displayed centered and to do so, texture coordinates are used.
	 *  - One of the bin is not displayed (the one at u=0.5f) because
	 *    it is neither positive freq, nor negative ones, but both. To
	 *    compensate for this, the (1.0f - tw) factor is used.
	 *
	 * Vertex mapping notes:
	 *
	 *  - We want the vertex to appear at the center of the displayed bins
	 *  - The vertex 'X' coordinates are filled in by the display kernel as
	 *    ((bin #) ^ (N >> 1)) / (N >> 1) - 1
	 *  - So the DC bin is 0.0f and the undisplayed bin is -1. The others
	 *    are spread between [ -1+2*tw  to  1-2*tw ]
	 *  - For display, that range is first remapped to [0 to 1], then to
	 *    [ bw/2 to 1-bw/2 ] (where bw is normalized displayed bin width)
	 *    so that each point maps to the center of the bin on the textures.
	 *  - Finally the zoom is applied and then the transform to map on the
	 *    requested screen area
	 */

        /* Draw waterfall */
	if (render->options & FRO_WATERFALL)
	{
		x[0] = render->_x[0];
		x[1] = render->_x[1];

		y[0] = render->_y_wf[0];
		y[1] = render->_y_wf[1];

		u[0] = 0.5f + tw + ((1.0f - tw) * render->freq_start);
		u[1] = 0.5f + tw + ((1.0f - tw) * render->freq_stop);

		v[1] = (float)render->_wf_pos / 1024.0f;
		v[0] = v[1] - render->wf_span;

		fosphor_gl_cmap_enable(gl->cmap_ctx,
		                       gl->tex_waterfall, gl->cmap_waterfall,
		                       self->power.scale, self->power.offset,
		                       GL_CMAP_MODE_BILINEAR);

		glBegin( GL_QUADS );
		glTexCoord2f(u[0], v[0]); glVertex2f(x[0], y[0]);
		glTexCoord2f(u[1], v[0]); glVertex2f(x[1], y[0]);
		glTexCoord2f(u[1], v[1]); glVertex2f(x[1], y[1]);
		glTexCoord2f(u[0], v[1]); glVertex2f(x[0], y[1]);
		glEnd();

		fosphor_gl_cmap_disable();

		if (render->options & FRO_COLOR_SCALE)
			fosphor_gl_cmap_draw_scale(gl->cmap_waterfall,
						   x[1]+2.0f, x[1]+10.0f, y[0], y[1]);
	}

	/* Draw histogram */
	if (render->options & FRO_HISTO)
	{
		x[0] = render->_x[0];
		x[1] = render->_x[1];

		y[0] = render->_y_histo[0];
		y[1] = render->_y_histo[1];

		u[0] = 0.5f + tw + ((1.0f - tw) * render->freq_start);
		u[1] = 0.5f + tw + ((1.0f - tw) * render->freq_stop);

		v[0] = 0.0f;
		v[1] = 1.0f;

		fosphor_gl_cmap_enable(gl->cmap_ctx,
		                       gl->tex_histogram, gl->cmap_histogram,
		                       1.1f, 0.0f, GL_CMAP_MODE_BILINEAR);

		glBegin( GL_QUADS );
		glTexCoord2f(u[0], v[0]); glVertex2f(x[0], y[0]);
		glTexCoord2f(u[1], v[0]); glVertex2f(x[1], y[0]);
		glTexCoord2f(u[1], v[1]); glVertex2f(x[1], y[1]);
		glTexCoord2f(u[0], v[1]); glVertex2f(x[0], y[1]);
		glEnd();

		fosphor_gl_cmap_disable();

		if (render->options & FRO_COLOR_SCALE)
			fosphor_gl_cmap_draw_scale(gl->cmap_histogram,
						   x[1]+2.0f, x[1]+10.0f, y[0], y[1]);
	}
	else if (render->options & (FRO_LIVE | FRO_MAX_HOLD))
	{
		x[0] = render->_x[0];
		x[1] = render->_x[1];

		y[0] = render->_y_histo[0];
		y[1] = render->_y_histo[1];

		glColor3f(0.0f, 0.0f, 0.1f);

		glBegin( GL_QUADS );
		glVertex2f(x[0], y[0]);
		glVertex2f(x[1], y[0]);
		glVertex2f(x[1], y[1]);
		glVertex2f(x[0], y[1]);
		glEnd();
	}

	/* Draw spectrum */
	if (render->options & (FRO_LIVE | FRO_MAX_HOLD))
	{
		int idx[2], len;

		/* Select end-points */
		idx[0] = 1 + (int)ceilf (render->freq_start * (float)(FOSPHOR_FFT_LEN - 1) - 0.5f);
		idx[1] = 1 + (int)floorf(render->freq_stop  * (float)(FOSPHOR_FFT_LEN - 1) - 0.5f);
		len = idx[1] - idx[0] + 1;

		/* Setup */
		glPushMatrix();

			/* Screen position scaling */
		glTranslatef(
			render->_x[0],
			render->_y_histo[0],
			0.0f
		);

		glScalef(
			render->_x[1] - render->_x[0],
			render->_y_histo[1] - render->_y_histo[0],
			1.0f
		);

			/* Power offset / scaling */
		glScalef(1.0f, self->power.scale, 1.0f);
		glTranslatef(0.0f, self->power.offset, 0.0f);

			/* Spectrum range selection */
		glScalef(1.0f / (render->freq_stop - render->freq_start), 1.0f, 1.0f);
		glTranslatef(-render->freq_start, 0.0f, 0.0f);

			/* Map the center of each N-1 bins */
		glTranslatef(0.5f * bw, 0.0f, 0.0f);
		glScalef(1.0f - bw, 1.0f, 1.0f);

			/* Spectrum x scaling to [0.0 -> 1.0] range */
		glTranslatef(0.5f, 0.0f, 0.0f);
		glScalef(0.5f / (1.0f - 2.0f * tw), 1.0f, 1.0f);

			/* GL state setup */
		glBindBuffer(GL_ARRAY_BUFFER, gl->vbo_spectrum);
		glVertexPointer(2, GL_FLOAT, 0, 0);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_LINE_SMOOTH);
		glLineWidth(1.0f);

		/* Live */
		if (render->options & FRO_LIVE)
		{
			glColor4f(1.0f, 1.0f, 1.0f, 0.75f);

			glEnableClientState(GL_VERTEX_ARRAY);
			glDrawArrays(GL_LINE_STRIP, idx[0], len);
			glDisableClientState(GL_VERTEX_ARRAY);
		}

		/* Max hold */
		if (render->options & FRO_MAX_HOLD)
		{
			glColor4f(1.0f, 0.0f, 0.0f, 0.75f);

			glEnableClientState(GL_VERTEX_ARRAY);
			glDrawArrays(GL_LINE_STRIP, idx[0] + FOSPHOR_FFT_LEN, len);
			glDisableClientState(GL_VERTEX_ARRAY);
		}

		/* Cleanup */
		glDisable(GL_BLEND);

		glPopMatrix();
	}

	/* Setup frequency axis */
	if (render->freq_start != 0.0f || render->freq_stop != 1.0f)
	{
		/* The freq_{start,stop} have some imprecisions due to the floating
		 * point nature. To avoid this crapping the display of the axis, we
		 * try to 'round' them */

		double freq_start = round(1e7 * render->freq_start) / 1e7;
		double freq_stop  = round(1e7 * render->freq_stop)  / 1e7;

		double rel_center = ((freq_stop + freq_start) / 2.0) - 0.5;
		double rel_span   =  (freq_stop - freq_start);

		freq_axis_build(&freq_axis,
				self->frequency.center + rel_center * self->frequency.span,
				self->frequency.span * rel_span,
				render->freq_n_div
		);
	}
	else
	{
		/* Use the straight number we were provider without math to
		 * avoid any imprecisions */
		freq_axis_build(&freq_axis,
				self->frequency.center,
				self->frequency.span,
				render->freq_n_div
		);
	}

	/* Draw grid */
	if (render->options & (FRO_LIVE | FRO_MAX_HOLD | FRO_HISTO))
	{
		for (i=0; i<11; i++)
		{
			float fg_color[3] = { 1.00f, 1.00f, 0.33f };
			float yv;

			yv = render->_y_histo[0] + i * render->_y_histo_div;

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor4f(0.0f, 0.0f, 0.0f, 0.5f);

			glBegin(GL_LINES);
			glVertex2f(render->_x[0] + 0.5f, yv + 0.5f);
			glVertex2f(render->_x[1] - 0.5f, yv + 0.5f);
			glEnd();

			glDisable(GL_BLEND);

			if (render->options & FRO_LABEL_PWR)
			{
				glf_begin(gl->font, fg_color);

				glf_printf(gl->font,
				           render->_x_label, GLF_RIGHT,
				           yv, GLF_CENTER,
				           "%d", self->power.db_ref - (10-i) * self->power.db_per_div
				);

				glf_end();
			}
		}

		for (i=0; i<=render->freq_n_div; i++)
		{
			float fg_color[3] = { 1.00f, 1.00f, 0.33f };
			float xv, xv_ofs, xv_ofs_total;
			char buf[32];

			xv = render->_x[0] + i * render->_x_div;

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor4f(0.0f, 0.0f, 0.0f, 0.5f);

			glBegin(GL_LINES);
			glVertex2f(xv + 0.5f, render->_y_histo[0]+0.5f);
			glVertex2f(xv + 0.5f, render->_y_histo[1]-0.5f);
			glEnd();

			glDisable(GL_BLEND);

			freq_axis_render(&freq_axis, buf,  (render->freq_n_div / 2));
			xv_ofs_total  = glf_width_str(gl->font, buf);
			freq_axis_render(&freq_axis, buf, -(render->freq_n_div / 2));
			xv_ofs_total += glf_width_str(gl->font, buf);
			xv_ofs_total /= 2.0f;

			if (render->options & FRO_LABEL_FREQ)
			{
				int ib = i - (render->freq_n_div / 2);

				glf_begin(gl->font, fg_color);

				freq_axis_render(&freq_axis, buf, ib);

				xv_ofs = floor((- xv_ofs_total * ib) / render->freq_n_div);

				glf_printf(gl->font,
				           xv + xv_ofs, GLF_CENTER,
				           render->_y_label, GLF_CENTER,
				           "%s", buf
				);

				glf_end();
			}
		}
	}

	/* Draw channels */
	if (render->options & FRO_CHANNELS)
	{
		struct {
			int   dir;
			float pos;
		} pt[2*FOSPHOR_MAX_CHANNELS+2], tpt;

		int i, j, n;

		/* Generate the points from the channels */
		n = 2;

		pt[0].dir = -1; pt[0].pos = 0.0f;
		pt[1].dir =  1; pt[1].pos = 1.0f;

		for (i=0; i<FOSPHOR_MAX_CHANNELS; i++)
		{
			float f;

			if (!render->channels[i].enabled)
				continue;

			f = render->channels[i].center
				- render->channels[i].width / 2.0f;
			pt[n].dir = 1;
			pt[n].pos = (f > 0.0f) ? (f < 1.0f ? f : 1.0f) : 0.0f;
			n++;

			f = render->channels[i].center
				+ render->channels[i].width / 2.0f;

			pt[n].dir = -1;
			pt[n].pos = (f > 0.0f) ? (f < 1.0f ? f : 1.0f) : 0.0f;
			n++;
		}

		/* Only if there is something to do ... */
		if (n > 2)
		{
			int l = pt[0].dir;

			/* GL setup */
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glPushMatrix();

			glTranslatef(render->_x[0], 0.0f, 0.0f);
			glScalef(render->_x[1] - render->_x[0], 1.0f, 1.0f);

			/* Sort and draw at the same time */
			for (i=1; i<n; i++)
			{
				int mi = i;

				/* Find min index */
				for (j=i+1; j<n; j++) {
					if (pt[j].pos < pt[mi].pos)
						mi = j;
				}

				/* Swap */
				tpt    = pt[i];
				pt[i]  = pt[mi];
				pt[mi] = tpt;

				/* Draw */
				if ((pt[i-1].pos != pt[i].pos) && (l != 0))
				{
					if (l < 0)
						glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
					else
						glColor4f(1.0f, 1.0f, 1.0f, 0.2f - 0.2f / (1 + l));

					if (render->options & FRO_WATERFALL) {
						glBegin( GL_QUADS );
						glVertex2f(pt[i  ].pos, render->_y_histo[0]);
						glVertex2f(pt[i-1].pos, render->_y_histo[0]);
						glVertex2f(pt[i-1].pos, render->_y_histo[1]);
						glVertex2f(pt[i  ].pos, render->_y_histo[1]);
						glEnd();
					}

					if (render->options & (FRO_LIVE | FRO_MAX_HOLD | FRO_HISTO)) {
						glBegin( GL_QUADS );
						glVertex2f(pt[i  ].pos, render->_y_wf[0]);
						glVertex2f(pt[i-1].pos, render->_y_wf[0]);
						glVertex2f(pt[i-1].pos, render->_y_wf[1]);
						glVertex2f(pt[i  ].pos, render->_y_wf[1]);
						glEnd();
					}
				}

				l += pt[i].dir;
			}

			/* GL cleanup */
			glPopMatrix();

			glDisable(GL_BLEND);
		}
	}

	/* Ensure GL is done */
	/* Make this optional.  If after the draw we do a swap buffer, we _know_
	   that GL will be done after it
	   Also, if we do multiple draw, then this is completely useless
	 */
	/* glFinish(); */
}

/*! @} */
