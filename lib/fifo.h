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

#include <gnuradio/gr_complex.h>
#include <gnuradio/thread/thread.h>

namespace gr {
  namespace fosphor {

   class GR_FOSPHOR_API fifo
   {
    private:
     gr_complex *d_buf;
     int d_len;
     int d_rp;
     int d_wp;

     thread::mutex d_mutex;
     thread::condition_variable d_cond_empty;
     thread::condition_variable d_cond_full;

    public:
     fifo(int length);
     ~fifo();

     int free();
     int used();

     int write_max_size();
     gr_complex *write_prepare(int size, bool wait=true);
     void write_commit(int size);

     int read_max_size();
     gr_complex *read_peek(int size, bool wait=true);
     void read_discard(int size);
   };

  } // namespace fosphor
} // namespace gr
