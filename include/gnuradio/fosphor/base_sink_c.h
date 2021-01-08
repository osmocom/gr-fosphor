/* -*- c++ -*- */
/*
 * Copyright 2013-2021 Sylvain Munaut <tnt@246tNt.com>
 *
 * This file is part of gr-fosphor
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <gnuradio/fosphor/api.h>

#include <gnuradio/sync_block.h>
#include <gnuradio/fft/window.h>

namespace gr {
  namespace fosphor {

    /*!
     * \brief Base fosphor sink API interface
     * \ingroup fosphor
     */
    class GR_FOSPHOR_API base_sink_c : public gr::sync_block
    {
     protected:
      base_sink_c(const char *name = NULL);

     public:

      enum ui_action_t {
        DB_PER_DIV_UP,
        DB_PER_DIV_DOWN,
        REF_UP,
        REF_DOWN,
        ZOOM_TOGGLE,
        ZOOM_WIDTH_UP,
        ZOOM_WIDTH_DOWN,
        ZOOM_CENTER_UP,
        ZOOM_CENTER_DOWN,
        RATIO_UP,
        RATIO_DOWN,
        FREEZE_TOGGLE,
      };

      enum mouse_action_t {
        CLICK,
      };

      virtual void execute_ui_action(enum ui_action_t action) = 0;
      virtual void execute_mouse_action(enum mouse_action_t action, int x, int y) = 0;

      virtual void set_frequency_range(const double center,
                                       const double span) = 0;
      virtual void set_frequency_center(const double center) = 0;
      virtual void set_frequency_span(const double span) = 0;

      virtual void set_fft_window(const gr::fft::window::win_type win) = 0;
    };

  } // namespace fosphor
} // namespace gr
