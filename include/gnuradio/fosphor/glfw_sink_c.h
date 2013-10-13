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


#ifndef INCLUDED_GR_FOSPHOR_SINK_C_H
#define INCLUDED_GR_FOSPHOR_SINK_C_H

#include <gnuradio/fosphor/api.h>

#include <gnuradio/sync_block.h>
#include <gnuradio/thread/thread.h>

struct fosphor;
struct GLFWwindow;

namespace gr {
  namespace fosphor {

    class fifo;

    /*!
     * \brief Main fosphor sink for GL spectrum display
     * \ingroup fosphor
     */
    class GR_FOSPHOR_API glfw_sink_c : public gr::sync_block
    {
     private:
      glfw_sink_c();

      void worker();
      static void _worker(glfw_sink_c *obj);

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
      typedef boost::shared_ptr<glfw_sink_c> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of fosphor::glfw_sink_c.
       *
       * To avoid accidental use of raw pointers, fosphor::glfw_sink_c's
       * constructor is in a private implementation
       * class. fosphor::glfw_sink_c::make is the public interface for
       * creating new instances.
       */
      static sptr make();

      ~glfw_sink_c();

      int work (int noutput_items,
                gr_vector_const_void_star &input_items,
                gr_vector_void_star &output_items);

    };

  } // namespace fosphor
} // namespace gr

#endif /* INCLUDED_GR_FOSPHOR_SINK_C_H */

