/* -*- c++ -*- */
/*
 * Copyright 2013-2014 Sylvain Munaut <tnt@246tNt.com>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
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
