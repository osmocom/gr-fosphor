# Copyright 2011-2020 Free Software Foundation, Inc.
# Copyright 2013-2021 Sylvain Munaut <tnt@246tNt.com>
#
# This file is part of gr-fosphor
#
# SPDX-License-Identifier: GPL-3.0-or-later

########################################################################
# Install public header files
########################################################################
list(APPEND fosphor_headers
    api.h
    base_sink_c.h
    overlap_cc.h
)

list_cond_append(ENABLE_GLFW fosphor_headers glfw_sink_c.h)
list_cond_append(ENABLE_QT   fosphor_headers qt_sink_c.h)

install(FILES
    ${fosphor_headers}
    DESTINATION include/gnuradio/fosphor
)
