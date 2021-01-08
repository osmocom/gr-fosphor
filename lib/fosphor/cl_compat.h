/*
 * cl_compat.h
 *
 * Handle OpenCL 1.1 <> 1.2 fallback and the related uglyness
 *
 * Copyright (C) 2013-2021 Sylvain Munaut
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

/*! \ingroup cl
 * @{
 */

/*! \file cl_compat.h
 *  \brief Handle OpenCL 1.1 <> 1.2 fallback and the related uglyness
 */

#include "cl_platform.h"
#include "gl_platform.h"


/* Define NVidia specific attributes */
#ifndef CL_DEVICE_COMPUTE_CAPABILITY_MAJOR_NV
# define CL_DEVICE_COMPUTE_CAPABILITY_MAJOR_NV	0x4000
# define CL_DEVICE_COMPUTE_CAPABILITY_MINOR_NV	0x4001
#endif


/* If OpenCL 1.2 isn't supported in the header, add our prototypes */
#ifndef CL_VERSION_1_2

typedef struct _cl_image_desc {
	cl_mem_object_type image_type;
	size_t image_width;
	size_t image_height;
	size_t image_depth;
	size_t image_array_size;
	size_t image_row_pitch;
	size_t image_slice_pitch;
	cl_uint num_mip_levels;
	cl_uint num_samples;
	cl_mem buffer;
} cl_image_desc;

cl_mem CL_API_CALL
clCreateFromGLTexture(cl_context context,
                      cl_mem_flags flags,
                      GLenum texture_target,
                      GLint miplevel,
                      GLuint texture,
                      cl_int *errcode_ret);

cl_mem CL_API_CALL
clCreateImage(cl_context context,
              cl_mem_flags flags,
              const cl_image_format *image_format,
              const cl_image_desc *image_desc,
              void *host_ptr,
              cl_int *errcode_ret);

cl_int CL_API_CALL
clEnqueueFillBuffer(cl_command_queue command_queue,
                    cl_mem buffer,
                    const void *pattern,
                    size_t pattern_size,
                    size_t offset,
                    size_t size,
                    cl_uint num_events_in_wait_list,
                    const cl_event *event_wait_list,
                    cl_event *event);

cl_int CL_API_CALL
clEnqueueFillImage(cl_command_queue command_queue,
                   cl_mem image,
                   const void *fill_color,
                   const size_t *origin,
                   const size_t *region,
                   cl_uint num_events_in_wait_list,
                   const cl_event *event_wait_list,
                   cl_event *event);

#endif


/* The actual API */
void cl_compat_init(void);
void cl_compat_check_platform(cl_platform_id pl_id);


/*! @} */
