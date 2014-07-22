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


#ifndef INCLUDED_GR_FOSPHOR_WX_CORE_SINK_C_IMPL_H
#define INCLUDED_GR_FOSPHOR_WX_CORE_SINK_C_IMPL_H

#include <gnuradio/fosphor/wx_core_sink_c.h>

#include "base_sink_c_impl.h"

namespace gr {
  namespace fosphor {

    /*!
     * \brief WX version of fosphor sink (core implementation)
     * \ingroup fosphor
     */
    class wx_core_sink_c_impl : public wx_core_sink_c, public base_sink_c_impl
    {
     private:
      /* Delegation to python */
      void pycall(PyObject *cb);

      PyObject *d_cb_init;
      PyObject *d_cb_fini;
      PyObject *d_cb_swap;
      PyObject *d_cb_update;

     protected:
      /* Delegated implementation of GL context management */
      void glctx_init();
      void glctx_swap();
      void glctx_poll();
      void glctx_fini();
      void glctx_update();

     public:
      wx_core_sink_c_impl(PyObject *cb_init, PyObject *cb_fini,
                          PyObject *cb_swap, PyObject *cb_update);
      ~wx_core_sink_c_impl();

      void pycb_reshape(int width, int height);
    };

  } // namespace fosphor
} // namespace gr

#endif /* INCLUDED_GR_FOSPHOR_WX_CORE_SINK_C_IMPL_H */

