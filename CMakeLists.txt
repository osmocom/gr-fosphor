# Copyright 2011-2020 Free Software Foundation, Inc.
# Copyright 2013-2021 Sylvain Munaut <tnt@246tNt.com>
#
# This file is part of gr-fosphor
#
# SPDX-License-Identifier: GPL-3.0-or-later

########################################################################
# Project setup
########################################################################
cmake_minimum_required(VERSION 3.8)
project(gr-fosphor CXX C)
include(GNUInstallDirs)
enable_testing()

#policy setup
cmake_policy(SET CMP0011 NEW)
cmake_policy(SET CMP0071 NEW)


#select the release build type by default to get optimization flags
if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "Release")
   message(STATUS "Build type not specified: defaulting to release.")
endif(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "")

########################################################################
# GNURadio setup
########################################################################
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake/Modules)

# Find GNURadio
find_package(Gnuradio "3.9" REQUIRED runtime fft)

# Set the version information here
set(VERSION_MAJOR 3)
set(VERSION_API   9)
set(VERSION_ABI   0)
set(VERSION_PATCH git)
include(GrVersion) #setup version info

########################################################################
# Compiler specific setup
########################################################################
if((CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
    CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    AND NOT WIN32)
    #http://gcc.gnu.org/wiki/Visibility
    add_definitions(-fvisibility=hidden)
    #false positives
    add_definitions(-Wno-format-overflow)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_STANDARD 14)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_STANDARD 14)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_STANDARD 14)
else()
    message(WARNING "C++ standard could not be set because compiler is not GNU, Clang or MSVC.")
endif()

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_C_STANDARD 11)
elseif(CMAKE_C_COMPILER_ID MATCHES "Clang")
    set(CMAKE_C_STANDARD 11)
elseif(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_C_STANDARD 11)
else()
    message(WARNING "C standard could not be set because compiler is not GNU, Clang or MSVC.")
endif()

########################################################################
# Install directories
########################################################################
include(GrPlatform) #define LIB_SUFFIX

if(NOT CMAKE_MODULES_DIR)
  set(CMAKE_MODULES_DIR lib${LIB_SUFFIX}/cmake)
endif(NOT CMAKE_MODULES_DIR)

set(GR_INCLUDE_DIR      include)
set(GR_CMAKE_DIR        ${CMAKE_MODULES_DIR}/gnuradio)
set(GR_PKG_DATA_DIR     ${GR_DATA_DIR}/${CMAKE_PROJECT_NAME})
set(GR_PKG_DOC_DIR      ${GR_DOC_DIR}/${CMAKE_PROJECT_NAME})
set(GR_PKG_CONF_DIR     ${GR_CONF_DIR}/${CMAKE_PROJECT_NAME}/conf.d)
set(GR_PKG_LIBEXEC_DIR  ${GR_LIBEXEC_DIR}/${CMAKE_PROJECT_NAME})

########################################################################
# Find boost
########################################################################
find_package(Boost "1.65" COMPONENTS system chrono thread)

if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost required to compile gr-fosphor")
endif()

########################################################################
# Find gr-fosphor build dependencies
########################################################################

# Required
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL)
if(NOT OPENGL_FOUND)
	message(FATAL_ERROR "OpenGL required to compile gr-fosphor")
endif()

find_package(OpenCL)
if(NOT OpenCL_FOUND)
	message(FATAL_ERROR "OpenCL required to compile gr-fosphor")
endif()

find_package(Freetype)
if(NOT FREETYPE_FOUND)
	message(FATAL_ERROR "freetype2 required to compile gr-fosphor")
endif()

# Optional
find_package(GLFW3)

set(Qt5_REQUIRED_COMPONENTS Core Gui OpenGL)
find_package(Qt5 5.9.0 COMPONENTS ${Qt5_REQUIRED_COMPONENTS})
if (Qt5_FOUND)
       foreach(module ${Qt5_REQUIRED_COMPONENTS})
               list(APPEND Qt5_INCLUDE_DIRS ${Qt5${module}_INCLUDE_DIRS})
               list(APPEND Qt5_LIBRARIES ${Qt5${module}_LIBRARIES})
       endforeach(module)
endif (Qt5_FOUND)

find_package(PNG 1.6.19)

########################################################################
# Find gnuradio build dependencies
########################################################################

find_package(Doxygen)
find_package(PythonLibs 3)

########################################################################
# PyBind11 Related
########################################################################

find_package(pybind11 REQUIRED)

if (pybind11_FOUND)
    execute_process(
        COMMAND "${PYTHON_EXECUTABLE}" -c
        "try:\n import numpy\n import os\n inc_path = numpy.get_include()\n if os.path.exists(os.path.join(inc_path, 'numpy', 'arrayobject.h')):\n  print(inc_path, end='')\nexcept:\n pass"
        OUTPUT_VARIABLE PYTHON_NUMPY_INCLUDE_DIR)
endif (pybind11_FOUND)

########################################################################
# Setup doxygen option
########################################################################
if(DOXYGEN_FOUND)
    option(ENABLE_DOXYGEN "Build docs using Doxygen" ON)
else(DOXYGEN_FOUND)
    option(ENABLE_DOXYGEN "Build docs using Doxygen" OFF)
endif(DOXYGEN_FOUND)

########################################################################
# Setup the components
########################################################################

include(GrComponent)

GR_REGISTER_COMPONENT("PNG" ENABLE_PNG
    PNG_FOUND
)

GR_REGISTER_COMPONENT("Python" ENABLE_PYTHON
    PYTHONLIBS_FOUND pybind11_FOUND
)

GR_REGISTER_COMPONENT("GLFW" ENABLE_GLFW
    GLFW3_FOUND
)

GR_REGISTER_COMPONENT("QT" ENABLE_QT
    Qt5_FOUND
)

macro(list_cond_append cond list_name)
    if(${cond})
        list(APPEND ${list_name} ${ARGN})
    endif(${cond})
endmacro(list_cond_append)

########################################################################
# Create uninstall target
########################################################################
configure_file(
    ${CMAKE_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
@ONLY)

add_custom_target(uninstall
    ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(include/gnuradio/fosphor)
add_subdirectory(lib)

if(ENABLE_PYTHON)
    add_subdirectory(python)
    add_subdirectory(grc)
    add_subdirectory(apps)
endif(ENABLE_PYTHON)

add_subdirectory(docs)

########################################################################
# Print Summary
########################################################################
GR_PRINT_COMPONENT_SUMMARY()
MESSAGE(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")
