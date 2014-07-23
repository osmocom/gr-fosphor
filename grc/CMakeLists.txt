# Copyright 2011-2020 Free Software Foundation, Inc.
# Copyright 2013-2021 Sylvain Munaut <tnt@246tNt.com>
#
# This file is part of gr-fosphor
#
# SPDX-License-Identifier: GPL-3.0-or-later

list_cond_append(ENABLE_GLFW fosphor_grc fosphor_glfw_sink_c.block.yml)
list_cond_append(ENABLE_QT   fosphor_grc fosphor_qt_sink_c.block.yml)

install(FILES
    fosphor.tree.yml overlap_cc.block.yml ${fosphor_grc}
    DESTINATION share/gnuradio/grc/blocks
)
