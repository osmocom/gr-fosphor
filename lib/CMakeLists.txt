# Copyright 2011,2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Setup library
########################################################################
include(GrPlatform) #define LIB_SUFFIX
include(GrMiscUtils)

find_package(PythonInterp 2)

add_custom_command(
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/fosphor
  OUTPUT fosphor/resource_data.c
  COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/fosphor/
  COMMAND cp llist.h resource_internal.h ${CMAKE_CURRENT_BINARY_DIR}/fosphor/
  COMMAND ${PYTHON_EXECUTABLE} -B mkresources.py fft.cl display.cl cmap_simple.glsl cmap_bicubic.glsl cmap_fallback.glsl DroidSansMonoDotted.ttf > ${CMAKE_CURRENT_BINARY_DIR}/fosphor/resource_data.c
)

include_directories(
	${OPENGL_INCLUDE_DIRS}
	${OPENCL_INCLUDE_DIRS}
	${FREETYPE2_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)
link_directories(
	${OPENGL_LIBRARY_DIRS}
	${OPENCL_LIBRARY_DIRS}
	${FREETYPE2_LIBRARY_DIRS}
	${Boost_LIBRARY_DIRS}
)

if(ENABLE_PYTHON)
    add_definitions(-DENABLE_PYTHON)
    include_directories(${PYTHON_INCLUDE_DIRS})
endif(ENABLE_PYTHON)

if(ENABLE_GLFW)
    include_directories(${GLFW3_INCLUDE_DIRS})
    link_directories(${GLFW3_LIBRARY_DIRS})
endif(ENABLE_GLFW)

if(ENABLE_QT)
    set(CMAKE_AUTOMOC ON)
    include_directories(${QT_INCLUDE_DIRS})
endif(ENABLE_QT)

list(APPEND fosphor_sources
	fosphor/axis.c
	fosphor/cl.c
	fosphor/fosphor.c
	fosphor/gl.c
	fosphor/gl_cmap.c
	fosphor/gl_cmap_gen.c
	fosphor/gl_font.c
	fosphor/resource.c
	fosphor/resource_data.c
	fifo.cc
	base_sink_c_impl.cc
)

list_cond_append(ENABLE_GLFW fosphor_sources glfw_sink_c_impl.cc)
list_cond_append(ENABLE_QT   fosphor_sources QGLSurface.cc qt_sink_c_impl.cc)
list_cond_append(ENABLE_WX   fosphor_sources wx_core_sink_c_impl.cc)

list(APPEND fosphor_libraries 
	${OPENGL_LIBRARIES}
	${OPENCL_LIBRARIES}
	${FREETYPE2_LIBRARIES}
	${Boost_LIBRARIES}
	${GNURADIO_RUNTIME_LIBRARIES}
)

list_cond_append(ENABLE_PYTHON fosphor_libraries ${PYTHON_LIBRARY})
list_cond_append(ENABLE_GLFW fosphor_libraries ${GLFW3_LIBRARIES})
list_cond_append(ENABLE_QT   fosphor_libraries ${QT_LIBRARIES})

add_library(gnuradio-fosphor SHARED ${fosphor_sources})
target_link_libraries(gnuradio-fosphor ${fosphor_libraries})
set_target_properties(gnuradio-fosphor PROPERTIES DEFINE_SYMBOL "gnuradio_fosphor_EXPORTS")

GR_LIBRARY_FOO(gnuradio-fosphor)
