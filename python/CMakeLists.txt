# Copyright 2011-2020 Free Software Foundation, Inc.
# Copyright 2013-2021 Sylvain Munaut <tnt@246tNt.com>
#
# This file is part of gr-fosphor
#
# SPDX-License-Identifier: GPL-3.0-or-later

########################################################################
# Include python install macros
########################################################################
include(GrPython)
if(NOT PYTHONINTERP_FOUND)
    return()
endif()

add_subdirectory(bindings)

########################################################################
# Install python sources
########################################################################
list(APPEND fosphor_python
    __init__.py
)

GR_PYTHON_INSTALL(
    FILES
    ${fosphor_python}
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/fosphor
)

########################################################################
# Handle the unit tests
########################################################################
include(GrTest)

set(GR_TEST_TARGET_DEPS gnuradio-fosphor)
