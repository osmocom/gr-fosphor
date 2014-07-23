/*
 * cl_compat.c
 *
 * Handle OpenCL 1.1 <> 1.2 fallback and the related uglyness
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

/*! \addtogroup cl
 * @{
 */

/*! \file cl_compat.c
 *  \brief Handle OpenCL 1.1 <> 1.2 fallback and the related uglyness
 */

/* Include whatever is needed for dynamic symbol lookup */
#ifdef _WIN32
# include <Windows.h>
#else
# define _GNU_SOURCE
# include <dlfcn.h>
#endif

#include <stdio.h>
#include <string.h>

/* Make sure we allow OpenCL 1.1 fn without warnings */
#define CL_USE_DEPRECATED_OPENCL_1_1_APIS

#include "cl_compat.h"


/* -------------------------------------------------------------------------- */
/* Fallback magic                                                             */
/* -------------------------------------------------------------------------- */

#define ALT_WRAP(rtype, fn, arg_list, arg_call)		\
							\
	static CL_API_ENTRY rtype CL_API_CALL (* fn ## _icd) arg_list = NULL; \
	static rtype fn ## _alt arg_list;		\
							\
	CL_API_ENTRY rtype CL_API_CALL fn arg_list	\
	{						\
		if (g_allow_cl12 && fn ## _icd)		\
			return fn ## _icd arg_call;	\
		else					\
			return fn ## _alt arg_call;	\
	}						\
							\
	static rtype fn ## _alt arg_list

#define ALT_INIT(fn)					\
	*(void **)(&fn ## _icd) = _cl_icd_get_sym(#fn);	\
	if (!fn ## _icd)				\
		g_allow_cl12 = 0;


/*! \brief Whether to allow direct CL 1.2 usage or not */
static int g_allow_cl12 = 1;


/*! \brief Tries to find a function pointer to a given OpenCL function
 *  \param[in] fn The name of the function to lookup
 */
static void *
_cl_icd_get_sym(const char *fn)
{
#ifdef _WIN32
	static HMODULE h = NULL;
	if (!h)
		h = GetModuleHandle(L"OpenCL.dll");
	return GetProcAddress(h, fn);
#else
	return dlsym(RTLD_NEXT, fn);
#endif
}


/* -------------------------------------------------------------------------- */
/* Fallback implementations                                                   */
/* -------------------------------------------------------------------------- */
/* These are only valid for fosphor and might not cover all use cases ! */

ALT_WRAP(cl_mem,
         clCreateFromGLTexture,
             (cl_context context,
              cl_mem_flags flags,
              GLenum texture_target,
              GLint miplevel,
              GLuint texture,
              cl_int *errcode_ret),
             (context, flags, texture_target, miplevel, texture, errcode_ret)
)
{
	return clCreateFromGLTexture2D(
		context,
		flags,
		texture_target,
		miplevel,
		texture,
		errcode_ret
	);
}


/* -------------------------------------------------------------------------- */
/* Compat API control                                                         */
/* -------------------------------------------------------------------------- */

void
cl_compat_init(void)
{
	ALT_INIT(clCreateFromGLTexture)
}

void
cl_compat_check_platform(cl_platform_id pl_id)
{
	cl_int err;
	char buf[128];

	if (!g_allow_cl12)
		return;

	err = clGetPlatformInfo(pl_id, CL_PLATFORM_VERSION, sizeof(buf), buf, NULL);
	if (err != CL_SUCCESS) {
		fprintf(stderr, "[!] Failed to fetch platform version. Assume it can't do OpenCL 1.2\n");
		g_allow_cl12 = 0;
	}

	if (strncmp(buf, "OpenCL 1.2 ", 11)) {
		g_allow_cl12 = 0;
	}
}

/*! @} */
