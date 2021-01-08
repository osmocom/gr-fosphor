/*
 * gl_font.h
 *
 * Basic OpenGL font rendering
 *
 * Copyright (C) 2013-2021 Sylvain Munaut
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

/*! \defgroup gl/font
 *  @{
 */

/*! \file gl_font.h
 *  \brief Basic OpenGL font rendering
 */

#ifdef _MSC_VER
# define ATTR_FORMAT(a,b,c)
#else
# define ATTR_FORMAT(a,b,c) __attribute__((format(a,b,c)))
#endif


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
                const char *fmt, ...) ATTR_FORMAT(printf, 6, 7);

void glf_begin(const struct gl_font *glf, float fg_color[3]);
void glf_end(void);


/*! @} */
