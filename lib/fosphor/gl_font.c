/*
 * gl_font.h
 *
 * Basic OpenGL font rendering
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

/*! \addtogroup gl/font
 *  @{
 */

/*! \file gl_font.c
 *  \brief Basic OpenGL font rendering
 */

#include <errno.h>
#include <math.h>
#include <stdint.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_BITMAP_H
#include FT_LCD_FILTER_H

#include "gl_platform.h"

#include "gl_font.h"


#define GLF_MIN_CHR	32	/*!< Minimum ascii char we display */
#define GLF_MAX_CHR	127	/*!< Maximum ascii char we display */
#define GLF_N_CHR	(GLF_MAX_CHR - GLF_MIN_CHR + 1)

struct gl_glyph
{
	FT_Glyph glyph;
	int advance_x;
};

struct gl_font
{
	/* Book keeping */
	int loaded;

	int height;
	int flags;

	/* Free type stuff */
	FT_Library library;			/*!< */
	FT_Face face;				/*!< */

	struct gl_glyph glyphs[GLF_N_CHR];

	/* GL stuff */
	GLuint vbo;

	/* Texture */
	struct {
		int width;
		int height;
		GLuint id;
	} tex;

	/* Glyphs */
	struct {
		int width;
		int height;
		int ofs_x;
		int ofs_y;
	} glyph_bb;
};


struct gl_font *
glf_alloc(int height, int flags)
{
	struct gl_font *glf;
	FT_Error ftr;

	glf = calloc(1, sizeof(struct gl_font));
	if (!glf)
		return NULL;

	ftr = FT_Init_FreeType(&glf->library);
	if (ftr)
		goto err;

	if (flags & GLF_FLG_LCD)
		FT_Library_SetLcdFilter(glf->library, FT_LCD_FILTER_DEFAULT);

	glf->height = height;
	glf->flags  = flags;

	glGenBuffers(1, &glf->vbo);

	return glf;

err:
	glf_free(glf);

	return NULL;
}

void
glf_free(struct gl_font *glf)
{
	if (!glf)
		return;

	if (glf->loaded) {
		int i;

		for (i=0; i<GLF_N_CHR; i++)
			if (glf->glyphs[i].glyph)
				FT_Done_Glyph(glf->glyphs[i].glyph);

		glDeleteTextures(1, &glf->tex.id);
	}

	glDeleteBuffers(1, &glf->vbo);

	if (glf->face)
		FT_Done_Face(glf->face);

	if (glf->library)
		FT_Done_FreeType(glf->library);

	free(glf);
}

static int
np2(int x)
{
	int r = 1;
	while (r && r < x)
		r <<= 1;
	return r;
}

static int
_glf_init_glyph(struct gl_font *glf, int ch, uint8_t *data)
{
	FT_Glyph glyph = NULL;
	FT_BitmapGlyph bitmap_glyph;
	FT_Bitmap bitmap;
	FT_Error ftr;
	int tx, ty, px, py;
	int x, y, m, rv;

	ftr = FT_Load_Glyph(glf->face, FT_Get_Char_Index(glf->face, ch), FT_LOAD_DEFAULT);
	if (ftr) {
		rv = -ENOMEM;
		goto err;
	}

	ftr = FT_Get_Glyph(glf->face->glyph, &glyph);
	if (ftr) {
		rv = -ENOMEM;
		goto err;
	}

	FT_Glyph_To_Bitmap(
		&glyph,
		glf->flags & GLF_FLG_LCD ?
			FT_RENDER_MODE_LCD : FT_RENDER_MODE_NORMAL,
		0, 1
	);

	bitmap_glyph = (FT_BitmapGlyph)glyph;
	bitmap = bitmap_glyph->bitmap;

	m = glf->flags & GLF_FLG_LCD ? 3 : 1;

	px = ((ch - GLF_MIN_CHR) & 15) * glf->glyph_bb.width;
	py = ((ch - GLF_MIN_CHR) >> 4) * glf->glyph_bb.height;

	for (y=0; y<bitmap.rows; y++) {
		for (x=0; x<bitmap.width; x++) {
			tx = m * (px +                        (glf->glyph_bb.ofs_x + bitmap_glyph->left)) + x;
			ty =      py + glf->glyph_bb.height - (glf->glyph_bb.ofs_y + bitmap_glyph->top)   + y;

			data[m * glf->tex.width * ty + tx] =
				bitmap.buffer[bitmap.pitch*y + x];
		}
	}

	glf->glyphs[ch - GLF_MIN_CHR].glyph = glyph;
	glf->glyphs[ch - GLF_MIN_CHR].advance_x = glf->face->glyph->advance.x >> 6;

	return 0;

err:
	if (glyph)
		FT_Done_Glyph(glyph);

	return rv;
}

static int
_glf_init_face(struct gl_font *glf)
{
	int min_x, max_x, min_y, max_y;
	uint8_t *data;
	int i;

	/* Set request size */
	FT_Set_Char_Size(glf->face, glf->height << 6, glf->height << 6, 96, 96);

	/* Find char BB and select texture size */
	min_x = FT_MulFix(glf->face->bbox.xMin, glf->face->size->metrics.x_scale) >> 6;
	max_x = FT_MulFix(glf->face->bbox.xMax, glf->face->size->metrics.x_scale) >> 6;
	min_y = FT_MulFix(glf->face->bbox.yMin, glf->face->size->metrics.x_scale) >> 6;
	max_y = FT_MulFix(glf->face->bbox.yMax, glf->face->size->metrics.x_scale) >> 6;

	glf->glyph_bb.width  = max_x - min_x + 1;
	glf->glyph_bb.height = max_y - min_y + 1;
	glf->glyph_bb.ofs_x  = - min_x;
	glf->glyph_bb.ofs_y  = - min_y;

	glf->tex.width  = np2(glf->glyph_bb.width  * 16);
	glf->tex.height = np2(glf->glyph_bb.height * (GLF_N_CHR + 15) >> 4);

	/* Raw data array */
	data = calloc(glf->tex.width * glf->tex.height, glf->flags & GLF_FLG_LCD ? 3 : 1);
	if (!data)
		return -ENOMEM;

	/* Init all glyphs */
	for (i=GLF_MIN_CHR; i<=GLF_MAX_CHR; i++)
		_glf_init_glyph(glf, i, data);

	/* Create GL texture */
	glGenTextures(1, &glf->tex.id);

	glBindTexture(GL_TEXTURE_2D, glf->tex.id);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

	if (glf->flags & GLF_FLG_LCD) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,   glf->tex.width, glf->tex.height, 0,
				GL_RGB,  GL_UNSIGNED_BYTE,  data);
	} else {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, glf->tex.width, glf->tex.height, 0,
				GL_ALPHA, GL_UNSIGNED_BYTE, data);
	}

	/* Done */
	free(data);

	glf->loaded = 1;

	return 0;
}

int
glf_load_face_file(struct gl_font *glf, const char *filename)
{
	FT_Error ftr;

	if (glf->loaded)
		return -EBUSY;

	ftr = FT_New_Face(glf->library, filename, 0, &glf->face);
	if (ftr)
		return -EINVAL;

	return _glf_init_face(glf);
}

int
glf_load_face_mem(struct gl_font *glf, const void *data, size_t len)
{
	FT_Error ftr;

	if (glf->loaded)
		return -EBUSY;

	ftr = FT_New_Memory_Face(glf->library, data, len, 0, &glf->face);
	if (ftr)
		return -EINVAL;

	return _glf_init_face(glf);
}

static void
_glf_add_char(const struct gl_font *glf, float *data, char c, float x)
{
	float u0, v0, u1, v1;
	float cw, ch, crw, crh;

	c -= GLF_MIN_CHR;

	cw  = (float)glf->glyph_bb.width;
	ch  = (float)glf->glyph_bb.height;
	crw = cw / (float)glf->tex.width;
	crh = ch / (float)glf->tex.height;

	u0 = (c & 15) * crw;
	v0 = (c >> 4) * crh;
	u1 = u0 + crw;
	v1 = v0 + crh;

	#define VTX(x,y,u,v) do { \
		data[0] = data[1] = data[2] = 1.0f; data[3] = 0.0f;	\
		data[4] = x; data[5] = y; data[6] = u, data[7] = v;	\
		data += 8;						\
	} while (0)

	VTX(x,      0.0f, u0, v1);
	VTX(x + cw, 0.0f, u1, v1);
	VTX(x + cw, ch,   u1, v0);
	VTX(x,      ch,   u0, v0);

	#undef VTX
}

void
glf_draw_str(const struct gl_font *glf,
             float x, enum glf_align x_align,
	     float y, enum glf_align y_align,
	     const char *str)
{
	float *data;
	float xb, xofs, yofs;
	int i;

	/* Temporary buffer for vertex data */
	data = malloc(8 * sizeof(float) * 4 * strlen(str));

	/* Add chars to the buffer */
	xb = 0.0f;

	for (i=0; str[i]; i++) {
		_glf_add_char(glf, &data[32*i], str[i], xb);
		xb += (float)glf->glyphs[str[i] - GLF_MIN_CHR].advance_x;
	}

	/* Align */
	if (x_align == GLF_CENTER) {
		xofs = x - roundf(xb / 2.0f);
	} else if (x_align == GLF_RIGHT) {
		xofs = x - xb;
	} else {
		xofs = x;
	}

	xofs -= (float) glf->glyph_bb.ofs_x;

	if (y_align == GLF_TOP) {
		yofs = y - (float)glf->glyph_bb.height;
	} else if (y_align == GLF_CENTER) {
		yofs = y - roundf((float)glf->glyph_bb.height / 2.0f);
	} else {
		yofs = y;
	}

	yofs += (float) glf->glyph_bb.ofs_y;

	for (i=0; i<4*strlen(str); i++) {
		data[8*i + 4] += xofs;
		data[8*i + 5] += yofs;
	}

	/* Draw */
#if 1
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glColorPointer   (4, GL_FLOAT, 8 * sizeof(float), data + 0);
	glVertexPointer  (2, GL_FLOAT, 8 * sizeof(float), data + 4);
	glTexCoordPointer(2, GL_FLOAT, 8 * sizeof(float), data + 6);

	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glDrawArrays(GL_QUADS, 0,  4*strlen(str));

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
#else
        glBegin( GL_QUADS );
	for (i=0; i<4*strlen(str); i++) {
		glColor4f(data[8*i + 0], data[8*i + 1], data[8*i + 2], data[8*i + 3]);
		glTexCoord2f(data[8*i + 6], data[8*i + 7]);
		glVertex2f(data[8*i + 4], data[8*i + 5]);
	}
        glEnd();
#endif

	/* Done */
	free(data);
}

void
glf_begin(const struct gl_font *glf, float fg_color[3])
{
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);

	glBindTexture(GL_TEXTURE_2D, glf->tex.id);

	if (glf->flags & GLF_FLG_LCD) {
		glColor3f(1.0f, 1.0f, 1.0f);
		glBlendColor(fg_color[0], fg_color[1], fg_color[2], 0.0f);
		glBlendFunc(GL_CONSTANT_COLOR, GL_ONE_MINUS_SRC_COLOR);
	} else {
		glColor3f(fg_color[0], fg_color[1], fg_color[2]);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}

void
glf_end(void)
{
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
}

/*! @} */
