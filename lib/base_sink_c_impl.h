/* -*- c++ -*- */
/*
 * Copyright 2013-2021 Sylvain Munaut <tnt@246tNt.com>
 *
 * This file is part of gr-fosphor
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

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
      bool d_visible;
      bool d_active;
      bool d_frozen;

      void worker();
      static void _worker(base_sink_c_impl *obj);

      gr::thread::mutex d_render_mutex;

      /* fosphor core */
      fifo *d_fifo;

      struct fosphor *d_fosphor;
      struct fosphor_render *d_render_main;
      struct fosphor_render *d_render_zoom;

      void render();

      static gr::thread::mutex s_boot_mutex;

      /* settings refresh logic */
      enum {
        SETTING_DIMENSIONS      = (1 << 0),
        SETTING_POWER_RANGE     = (1 << 1),
        SETTING_FREQUENCY_RANGE = (1 << 2),
        SETTING_FFT_WINDOW      = (1 << 3),
        SETTING_RENDER_OPTIONS  = (1 << 4),
      };

      uint32_t d_settings_changed;
      gr::thread::mutex d_settings_mutex;

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
      void cb_visibility(bool visible);

     public:
      virtual ~base_sink_c_impl();

      /* gr::fosphor::base_sink_c implementation */
      void execute_ui_action(enum ui_action_t action);
      void execute_mouse_action(enum mouse_action_t action, int x, int y);

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
