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

#include <gnuradio/sync_interpolator.h>
#include <gnuradio/fft/window.h>

namespace gr {
  namespace fosphor {

    /*!
     * \brief Block preparing an overlapped version of the stream
     * \ingroup fosphor
     */
    class GR_FOSPHOR_API overlap_cc : virtual public gr::sync_interpolator
    {
     public:
      typedef std::shared_ptr<overlap_cc> sptr;

      static sptr make(int wlen, int overlap);

      virtual void set_overlap_ratio(const int overlap) = 0;
    };

  } // namespace fosphor
} // namespace gr
