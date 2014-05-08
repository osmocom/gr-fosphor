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

#include <stdint.h>

#include <gnuradio/thread/thread.h>

#include <gnuradio/fosphor/base_sink_c.h>

struct fosphor;
struct fosphor_render;

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
      bool d_frozen;

      void worker();
      static void _worker(base_sink_c_impl *obj);

      /* fosphor core */
      fifo *d_fifo;

      struct fosphor *d_fosphor;
      struct fosphor_render *d_render_main;
      struct fosphor_render *d_render_zoom;

      void render();

      /* settings refresh logic */
      enum {
        SETTING_DIMENSIONS	= (1 << 0),
        SETTING_POWER_RANGE	= (1 << 1),
        SETTING_FREQUENCY_RANGE	= (1 << 2),
        SETTING_FFT_WINDOW	= (1 << 3),
	SETTING_RENDER_OPTIONS	= (1 << 4),
      };

      uint32_t d_settings_changed;
      thread::mutex d_settings_mutex;

      void     settings_mark_changed(uint32_t setting);
      uint32_t settings_get_and_reset_changed(void);
      void     settings_apply(uint32_t settings);

      /* settings values */
      int d_width;
      int d_height;

      static const int k_db_per_div[];
      int d_db_ref;
      int d_db_per_div_idx;

      bool  d_zoom_enabled;
      double d_zoom_center;
      double d_zoom_width;

      float d_ratio;

      struct {
        double center;
        double span;
      } d_frequency;

      gr::fft::window::win_type d_fft_window;

     protected:
      base_sink_c_impl();

      /* Delegated implementation of GL context management */
      virtual void glctx_init() = 0;
      virtual void glctx_poll() = 0;
      virtual void glctx_swap() = 0;
      virtual void glctx_fini() = 0;
      virtual void glctx_update() = 0;

      /* Callbacks from GL window */
      void cb_reshape(int width, int height);

     public:
      virtual ~base_sink_c_impl();

      /* gr::fosphor::base_sink_c implementation */
      void execute_ui_action(enum ui_action_t action);

      void set_frequency_range(const double center,
                               const double span);
      void set_frequency_center(const double center);
      void set_frequency_span(const double span);

      void set_fft_window(const gr::fft::window::win_type win);

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

