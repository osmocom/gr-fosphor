#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2013 Sylvain Munaut <tnt@246tNt.com>
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

import numpy

import wx
import wx.glcanvas
from OpenGL import GL

from gnuradio import gr

from fosphor_swig import base_sink_c, wx_core_sink_c

import threading

class wx_sink_c(gr.hier_block2):

    def __init__(self, parent, size=(600,600)):
        gr.hier_block2.__init__(
            self,
            "wx_sink_c",
            gr.io_signature(1, 1, gr.sizeof_gr_complex),
            gr.io_signature(0, 0, 0),
        )

        # Create the GL Canvas (without context for now)
        attribList = (wx.glcanvas.WX_GL_DOUBLEBUFFER, wx.glcanvas.WX_GL_RGBA)
        self.win = wx.glcanvas.GLCanvas(parent, wx.ID_ANY,
            attribList=attribList,
            size=size,
            style=wx.WANTS_CHARS
        )

        self.win.Bind(wx.EVT_SIZE, self._evt_size)
        self.win.Bind(wx.EVT_KEY_DOWN, self._evt_key_down)
        self.win.SetFocus()

        self._gl_ctx = None

        # Create the underlying WX sink core
        self.sink = wx_core_sink_c(self._glctx_init, self._glctx_fini,
                                   self._glctx_swap, self._glctx_update)
        self.connect(self, self.sink)

    def _evt_size(self, evt):
        self.sink.pycb_reshape(*evt.GetSize())

    def _evt_key_down(self, evt):
        k = evt.GetKeyCode()
        if k == wx.WXK_UP:
            self.sink.execute_ui_action(base_sink_c.REF_DOWN)
        elif k == wx.WXK_DOWN:
            self.sink.execute_ui_action(base_sink_c.REF_UP)
        elif k == wx.WXK_LEFT:
            self.sink.execute_ui_action(base_sink_c.DB_PER_DIV_DOWN)
        elif k == wx.WXK_RIGHT:
            self.sink.execute_ui_action(base_sink_c.DB_PER_DIV_UP)
        else:
            evt.Skip()

    def _glctx_init(self):
        if self._gl_ctx is None:
            self._gl_ctx = wx.glcanvas.GLContext(self.win)
        self.win.SetCurrent(self._gl_ctx)

    def _glctx_fini(self):
        self._gl_ctx = None

    def _glctx_swap(self):
        self.win.SwapBuffers()

    def _glctx_update(self):
        self.win.SetCurrent(self._gl_ctx)

    def __getattr__(self, attr):
        try:
            return gr.hier_block2.__getattr__(self, attr)
        except:
            return getattr(self.sink, attr)

    def set_baseband_freq(self, bb_freq):
        self.set_frequency_center(bb_freq)

    def set_sample_rate(self, sample_rate):
        self.set_frequency_span(sample_rate)
