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
#include <gnuradio/fosphor/base_sink_c.h>

#include <gnuradio/sync_block.h>

namespace gr {
  namespace fosphor {

    /*!
     * \brief GLFW version of fosphor sink
     * \ingroup fosphor
     */
    class GR_FOSPHOR_API glfw_sink_c : virtual public base_sink_c
    {
     public:
      typedef std::shared_ptr<glfw_sink_c> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of fosphor::glfw_sink_c.
       *
       * To avoid accidental use of raw pointers, fosphor::glfw_sink_c's
       * constructor is in a private implementation
       * class. fosphor::glfw_sink_c::make is the public interface for
       * creating new instances.
       */
      static sptr make();
    };

  } // namespace fosphor
} // namespace gr
