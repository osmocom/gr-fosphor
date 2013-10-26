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


#ifndef INCLUDED_GR_FOSPHOR_WX_CORE_SINK_C_H
#define INCLUDED_GR_FOSPHOR_WX_CORE_SINK_C_H

#include <gnuradio/fosphor/api.h>
#include <gnuradio/fosphor/base_sink_c.h>

#include <gnuradio/sync_block.h>

namespace gr {
  namespace fosphor {

    /*!
     * \brief WX version of fosphor sink (core)
     * \ingroup fosphor
     */
    class GR_FOSPHOR_API wx_core_sink_c : virtual public base_sink_c
    {
     public:
      typedef boost::shared_ptr<wx_core_sink_c> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of fosphor::wx_core_sink_c.
       *
       * To avoid accidental use of raw pointers, fosphor::wx_core_sink_c's
       * constructor is in a private implementation
       * class. fosphor::wx_core_sink_c::make is the public interface for
       * creating new instances.
       */
      static sptr make(PyObject *cb_init, PyObject *cb_fini, PyObject *cb_swap);

      virtual void pycb_reshape(int width, int height) = 0;
    };

  } // namespace fosphor
} // namespace gr

#endif /* INCLUDED_GR_FOSPHOR_WX_CORE_SINK_C_H */

