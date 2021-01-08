/* -*- c++ -*- */
/*
 * Copyright 2013-2021 Sylvain Munaut <tnt@246tNt.com>
 *
 * This file is part of gr-fosphor
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <gnuradio/fosphor/glfw_sink_c.h>

#include "base_sink_c_impl.h"

struct GLFWwindow;

namespace gr {
  namespace fosphor {

    /*!
     * \brief GLFW version of fosphor sink (implementation)
     * \ingroup fosphor
     */
    class glfw_sink_c_impl : public glfw_sink_c, public base_sink_c_impl
    {
     private:
      /* GLFW stuff */
      GLFWwindow *d_window;

      void glfw_render(void);
      void glfw_cb_reshape(int w, int h);
      void glfw_cb_key(int key, int scancode, int action, int mods);
      void glfw_cb_mouse(int btn, int action, int mods);

      static void _glfw_cb_reshape(GLFWwindow *wnd, int w, int h);
      static void _glfw_cb_key(GLFWwindow *wnd, int key, int scancode, int action, int mods);
      static void _glfw_cb_mouse(GLFWwindow *wnd, int btn, int action, int mods);

     protected:
      /* Delegated implementation of GL context management */
      void glctx_init();
      void glctx_swap();
      void glctx_poll();
      void glctx_fini();
      void glctx_update();

     public:
      glfw_sink_c_impl();
    };

  } // namespace fosphor
} // namespace gr
