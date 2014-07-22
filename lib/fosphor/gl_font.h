/*
 * gl_font.h
 *
 * Basic OpenGL font rendering
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

#ifndef __FOSPHOR_GL_FONT_H__
#define __FOSPHOR_GL_FONT_H__

/*! \defgroup gl/font
 *  @{
 */

/*! \file gl_font.h
 *  \brief Basic OpenGL font rendering
 */


struct gl_font;

#define GLF_FLG_LCD	(1 << 0)

enum glf_align
{
	GLF_LEFT,
	GLF_RIGHT,
	GLF_TOP,
	GLF_BOTTOM,
	GLF_CENTER,
};

struct gl_font *glf_alloc(int height, int flags);
void glf_free(struct gl_font *glf);

int glf_load_face_file(struct gl_font *glf, const char *filename);
int glf_load_face_mem(struct gl_font *glf, const void *data, size_t len);

float glf_width_str(const struct gl_font *glf, const char *str);

void glf_draw_str(const struct gl_font *glf,
                  float x, enum glf_align x_align,
                  float y, enum glf_align y_align,
                  const char *str);

void glf_printf(const struct gl_font *glf,
                float x, enum glf_align x_align,
                float y, enum glf_align y_align,
                const char *fmt, ...) __attribute__((format(printf, 6, 7)));

void glf_begin(const struct gl_font *glf, float fg_color[3]);
void glf_end(void);


/*! @} */

#endif /* __FOSPHOR_GL_FONT_H__ */
