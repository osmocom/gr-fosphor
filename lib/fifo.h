/* -*- c++ -*- */
/*
 * Copyright 2013-2014 Sylvain Munaut <tnt@246tNt.com>
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

#ifndef INCLUDED_GR_FOSPHOR_FIFO_H
#define INCLUDED_GR_FOSPHOR_FIFO_H

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

#endif /* INCLUDED_GR_FOSPHOR_FIFO_H */
