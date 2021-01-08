/*
 * gl_platform.h
 *
 * Wrapper to select proper OpenGL headers for various platforms
 *
 * Copyright (C) 2013-2021 Sylvain Munaut
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/*! \file gl_platform.h
 *  \brief Wrapper to select proper OpenGL headers for various platforms
 */

#if !defined(_WIN32) && (defined(__WIN32__) || defined(WIN32) || defined(__CYGWIN__))
# define _WIN32
#endif

#if defined(__APPLE__) || defined(MACOSX)
# define GL_GLEXT_PROTOTYPES
# include <OpenGL/gl.h>
#elif defined(_WIN32)
# include <GL/glew.h>
#else
# define GL_GLEXT_PROTOTYPES
# include <GL/gl.h>
#endif
