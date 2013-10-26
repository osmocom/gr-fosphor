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


#ifndef INCLUDED_GR_FOSPHOR_BASE_SINK_C_IMPL_H
#define INCLUDED_GR_FOSPHOR_BASE_SINK_C_IMPL_H

#include <gnuradio/thread/thread.h>

#include <gnuradio/fosphor/base_sink_c.h>

struct fosphor;

namespace gr {
  namespace fosphor {

    class fifo;

    /*!
     * \brief Base class for fosphor sink implementation
     * \ingroup fosphor
     */
    class base_sink_c_impl : virtual public base_sink_c
    {
     private:
      /* Worker thread */
      gr::thread::thread d_worker;
      bool d_active;

      void worker();
      static void _worker(base_sink_c_impl *obj);

      /* fosphor core */
      fifo *d_fifo;

      struct fosphor *d_fosphor;

      void render();

      int d_width;
      int d_height;
      bool d_reshaped;

      static const int k_db_per_div[];
      int d_db_ref;
      int d_db_per_div_idx;

     protected:
      base_sink_c_impl();

      /* Delegated implementation of GL context management */
      virtual void glctx_init() = 0;
      virtual void glctx_poll() = 0;
      virtual void glctx_swap() = 0;
      virtual void glctx_fini() = 0;

      /* Callbacks from GL window */
      void cb_reshape(int width, int height);

     public:
      virtual ~base_sink_c_impl();

      /* gr::fosphor::base_sink_c implementation */
      void execute_ui_action(enum ui_action_t action);

      /* gr::sync_block implementation */
      int work (int noutput_items,
                gr_vector_const_void_star &input_items,
                gr_vector_void_star &output_items);

      bool start();
      bool stop();
    };

  } // namespace fosphor
} // namespace gr

#endif /* INCLUDED_GR_FOSPHOR_BASE_SINK_C_IMPL_H */

