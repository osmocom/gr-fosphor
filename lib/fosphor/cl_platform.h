/*
 * cl_platform.h
 *
 * Wrapper to select proper OpenCL headers for various platforms
 *
 * Copyright (C) 2013-2021 Sylvain Munaut
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/*! \file cl_platform.h
 *  \brief Wrapper to select proper OpenCL headers for various platforms
 */

#if !defined(_WIN32) && (defined(__WIN32__) || defined(WIN32) || defined(__CYGWIN__))
# define _WIN32
#endif

#define CL_TARGET_OPENCL_VERSION 120

#if defined(__APPLE__) || defined(MACOSX)
# include <OpenCL/cl.h>
# include <OpenCL/cl_ext.h>
# include <OpenCL/cl_gl.h>
# include <OpenCL/cl_gl_ext.h>
#else
# define CL_USE_DEPRECATED_OPENCL_1_1_APIS
# define CL_USE_DEPRECATED_OPENCL_1_2_APIS
# include <CL/cl.h>
# include <CL/cl_ext.h>
# include <CL/cl_gl.h>
#endif
