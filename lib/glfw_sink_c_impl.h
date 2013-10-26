/* -*- c++ -*- */
/*
 * Copyright 2013 Sylvain Munaut <tnt@246tNt.com>
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


#ifndef INCLUDED_GR_FOSPHOR_GLFW_SINK_C_IMPL_H
#define INCLUDED_GR_FOSPHOR_GLFW_SINK_C_IMPL_H

#include <gnuradio/thread/thread.h>

#include <gnuradio/fosphor/glfw_sink_c.h>

struct fosphor;
struct GLFWwindow;

namespace gr {
  namespace fosphor {

    class fifo;

    /*!
     * \brief Main fosphor sink for GL spectrum display
     * \ingroup fosphor
     */
    class glfw_sink_c_impl : public glfw_sink_c
    {
     private:
      void worker();
      static void _worker(glfw_sink_c_impl *obj);

      void glfw_render(void);
      void glfw_cb_reshape(int w, int h);
      void glfw_cb_key(int key, int scancode, int action, int mods);

      static void _glfw_cb_reshape(GLFWwindow *wnd, int w, int h);
      static void _glfw_cb_key(GLFWwindow *wnd, int key, int scancode, int action, int mods);

      gr::thread::thread d_worker;
      bool d_active;

      GLFWwindow *d_window;

      fifo *d_fifo;
      struct fosphor *d_fosphor;
      int d_width, d_height;
      int d_db_ref, d_db_per_div_idx;
      static const int k_db_per_div[];

     public:
      glfw_sink_c_impl();
      virtual ~glfw_sink_c_impl();

      int work (int noutput_items,
                gr_vector_const_void_star &input_items,
                gr_vector_void_star &output_items);

    };

  } // namespace fosphor
} // namespace gr

#endif /* INCLUDED_GR_FOSPHOR_GLFW_SINK_C_IMPL_H */

