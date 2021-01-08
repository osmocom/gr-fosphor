# Copyright 2011-2020 Free Software Foundation, Inc.
# Copyright 2013-2021 Sylvain Munaut <tnt@246tNt.com>
#
# This file is part of gr-fosphor
#
# SPDX-License-Identifier: GPL-3.0-or-later

########################################################################
# Check for pygccxml
########################################################################
GR_PYTHON_CHECK_MODULE_RAW(
    "pygccxml"
    "import pygccxml"
    PYGCCXML_FOUND
    )

# Official module is broken, makes too many assumptions that are not true
#include(GrPybind)

include(GrPython)

macro(GR_PYBIND_MAKE name updir filter files)

configure_file(${CMAKE_SOURCE_DIR}/docs/doxygen/pydoc_macros.h ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

pybind11_add_module(${name}_python ${files})

SET(MODULE_NAME ${name})
if (${name} STREQUAL gr)
    SET(MODULE_NAME "runtime")
endif()

target_include_directories(${name}_python PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PYTHON_NUMPY_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/${updir}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/${updir}/include
    ${PYBIND11_INCLUDE_DIR}
)
target_link_libraries(${name}_python PUBLIC ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} gnuradio-${MODULE_NAME})
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
   CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(${name}_python PRIVATE -Wno-unused-variable) # disable warnings for docstring templates
endif(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
      CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

endmacro(GR_PYBIND_MAKE)



########################################################################
# Python Bindings
########################################################################

list(APPEND fosphor_python_files
    base_sink_c_python.cc
    glfw_sink_c_python.cc
    qt_sink_c_python.cc
    python_bindings.cc)

GR_PYBIND_MAKE(fosphor
   ../..
   gr::fosphor
   "${fosphor_python_files}")

install(TARGETS fosphor_python DESTINATION ${GR_PYTHON_DIR}/gnuradio/fosphor COMPONENT pythonapi)
