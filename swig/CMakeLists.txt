# Copyright 2011-2020 Free Software Foundation, Inc.
# Copyright 2013-2021 Sylvain Munaut <tnt@246tNt.com>
#
# This file is part of gr-fosphor
#
# SPDX-License-Identifier: GPL-3.0-or-later

########################################################################
# Include swig generation macros
########################################################################
find_package(SWIG)
find_package(PythonLibs 3)
if(NOT SWIG_FOUND OR NOT PYTHONLIBS_FOUND)
    return()
endif()
include(GrSwig)
include(GrPython)

########################################################################
# Setup swig generation
########################################################################
set(GR_SWIG_INCLUDE_DIRS $<TARGET_PROPERTY:gnuradio::runtime_swig,INTERFACE_INCLUDE_DIRECTORIES>)
set(GR_SWIG_TARGET_DEPS gnuradio::runtime_swig)

set(GR_SWIG_LIBRARIES gnuradio-fosphor)

set(GR_SWIG_DOC_FILE ${CMAKE_CURRENT_BINARY_DIR}/fosphor_swig_doc.i)
set(GR_SWIG_DOC_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../include/gnuradio/fosphor)

list_cond_append(ENABLE_GLFW GR_SWIG_FLAGS -DENABLE_GLFW)
list_cond_append(ENABLE_QT   GR_SWIG_FLAGS -DENABLE_QT)

GR_SWIG_MAKE(fosphor_swig fosphor_swig.i)

########################################################################
# Install the build swig module
########################################################################
GR_SWIG_INSTALL(TARGETS fosphor_swig DESTINATION ${GR_PYTHON_DIR}/gnuradio/fosphor)

########################################################################
# Install swig .i files for development
########################################################################
install(
    FILES
    fosphor_swig.i
    ${CMAKE_CURRENT_BINARY_DIR}/fosphor_swig_doc.i
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig
)
