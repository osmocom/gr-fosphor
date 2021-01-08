# Copyright 2011-2020 Free Software Foundation, Inc.
# Copyright 2013-2021 Sylvain Munaut <tnt@246tNt.com>
#
# This file is part of gr-fosphor
#
# SPDX-License-Identifier: GPL-3.0-or-later

########################################################################
# Setup library
########################################################################
include(GrPlatform) #define LIB_SUFFIX

find_package(PythonInterp 3)

add_custom_command(
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/fosphor
  OUTPUT fosphor/resource_data.c
  DEPENDS fosphor/fft.cl fosphor/display.cl fosphor/cmap_simple.glsl fosphor/cmap_bicubic.glsl fosphor/cmap_fallback.glsl fosphor/DroidSansMonoDotted.ttf
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/fosphor/
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/fosphor/llist.h ${CMAKE_CURRENT_BINARY_DIR}/fosphor/
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/fosphor/resource_internal.h ${CMAKE_CURRENT_BINARY_DIR}/fosphor/
  COMMAND ${PYTHON_EXECUTABLE} -B mkresources.py fft.cl display.cl cmap_simple.glsl cmap_bicubic.glsl cmap_fallback.glsl DroidSansMonoDotted.ttf > ${CMAKE_CURRENT_BINARY_DIR}/fosphor/resource_data.c
)

list(APPEND fosphor_sources
	fosphor/axis.c
	fosphor/cl.c
	fosphor/cl_compat.c
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

add_library(gnuradio-fosphor SHARED ${fosphor_sources})

target_include_directories(gnuradio-fosphor
	PUBLIC ${OPENGL_INCLUDE_DIRS}
	PUBLIC ${OpenCL_INCLUDE_DIRS}
	PUBLIC ${FREETYPE_INCLUDE_DIRS}
	PUBLIC ${Boost_INCLUDE_DIR}
	PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
	PUBLIC $<INSTALL_INTERFACE:include>
)
target_link_libraries(gnuradio-fosphor
	${OPENGL_LIBRARIES}
	${OpenCL_LIBRARIES}
	${FREETYPE_LIBRARIES}
	${Boost_LIBRARIES}
	gnuradio::gnuradio-runtime
	gnuradio::gnuradio-fft
	${CMAKE_DL_LIBS}
)

if(WIN32)
   find_package(GLEW REQUIRED)
   target_include_directories(gnuradio-fosphor PRIVATE ${GLEW_INCLUDE_DIRS})
   target_link_libraries(gnuradio-fosphor ${GLEW_LIBRARIES})
   add_definitions(-DENABLE_GLEW)
endif(WIN32)

if(ENABLE_PYTHON)
    add_definitions(-DENABLE_PYTHON)
    target_include_directories(gnuradio-fosphor PUBLIC ${PYTHON_INCLUDE_DIRS})
    target_link_libraries(gnuradio-fosphor ${PYTHON_LIBRARY})
endif(ENABLE_PYTHON)

if(ENABLE_GLFW)
    target_include_directories(gnuradio-fosphor PUBLIC ${GLFW3_INCLUDE_DIRS})
    target_link_libraries(gnuradio-fosphor ${GLFW3_LIBRARIES})
endif(ENABLE_GLFW)

if(ENABLE_QT)
    set_target_properties(gnuradio-fosphor PROPERTIES AUTOMOC ON)
    target_include_directories(gnuradio-fosphor PUBLIC ${Qt5_INCLUDE_DIRS})
    target_link_libraries(gnuradio-fosphor ${Qt5_LIBRARIES})
endif(ENABLE_QT)

set_target_properties(gnuradio-fosphor PROPERTIES DEFINE_SYMBOL "gnuradio_fosphor_EXPORTS")

if(APPLE)
    set_target_properties(gnuradio-fosphor PROPERTIES
        INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
    )
endif(APPLE)

########################################################################
# Install built library files
########################################################################
include(GrMiscUtils)
GR_LIBRARY_FOO(gnuradio-fosphor)

########################################################################
# Print summary
########################################################################
message(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Building for version: ${VERSION} / ${LIBVER}")
