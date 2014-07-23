/* -*- c++ -*- */
/*
 * Copyright 2013-2021 Sylvain Munaut <tnt@246tNt.com>
 *
 * This file is part of gr-fosphor
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <gnuradio/fosphor/overlap_cc.h>

struct fosphor;
struct fosphor_render;

namespace gr {
  namespace fosphor {

    /*!
     * \brief Block preparing an overlapped version of the stream
     * \ingroup fosphor
     */
    class overlap_cc_impl : public overlap_cc
    {
     private:
      int d_wlen;
      int d_overlap;

     public:
      overlap_cc_impl(int wlen, int overlap);
      virtual ~overlap_cc_impl();

      void set_overlap_ratio(const int overlap);

      /* gr::sync_interpolator implementation */
      int work (int noutput_items,
                gr_vector_const_void_star &input_items,
                gr_vector_void_star &output_items);
    };

  } // namespace fosphor
} // namespace gr
