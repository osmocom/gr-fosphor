/*
 * cl_platform.h
 *
 * Wrapper to select proper OpenCL headers for various platforms
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

/*! \file cl_platform.h
 *  \brief Wrapper to select proper OpenCL headers for various platforms
 */

#if !defined(_WIN32) && (defined(__WIN32__) || defined(WIN32) || defined(__CYGWIN__))
# define _WIN32
#endif

#if defined(__APPLE__) || defined(MAXOSX)
# include <OpenCL/cl.h>
# include <OpenCL/cl_ext.h>
# include <OpenCL/cl_gl.h>
# include <OpenCL/cl_gl_ext.h>
#else
# include <CL/cl.h>
# include <CL/cl_ext.h>
# include <CL/cl_gl.h>
#endif

#ifdef CL_VERSION_1_2
# define clCreateFromGLTexture2D clCreateFromGLTexture
#endif

#ifndef CL_DEVICE_COMPUTE_CAPABILITY_MAJOR_NV
# define CL_DEVICE_COMPUTE_CAPABILITY_MAJOR_NV	0x4000
# define CL_DEVICE_COMPUTE_CAPABILITY_MINOR_NV	0x4001
#endif
