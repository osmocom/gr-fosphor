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


#ifndef INCLUDED_GR_FOSPHOR_QT_SINK_C_IMPL_H
#define INCLUDED_GR_FOSPHOR_QT_SINK_C_IMPL_H

#include <gnuradio/fosphor/qt_sink_c.h>

#include "base_sink_c_impl.h"

namespace gr {
  namespace fosphor {

    class QGLSurface;

    /*!
     * \brief Qt version of fosphor sink (implementation)
     * \ingroup fosphor
     */
    class qt_sink_c_impl : public qt_sink_c, public base_sink_c_impl
    {
     friend class QGLSurface;

     private:
      QGLSurface *d_gui;

     protected:
      /* Delegated implementation of GL context management */
      void glctx_init();
      void glctx_swap();
      void glctx_poll();
      void glctx_fini();
      void glctx_update();

     public:
      qt_sink_c_impl(QWidget *parent=NULL);

      void exec_();
      QWidget* qwidget();

#if defined(PY_VERSION) || defined(SWIGPYTHON)
      PyObject* pyqwidget();
#else
      void* pyqwidget();
#endif
    };

  } // namespace fosphor
} // namespace gr

#endif /* INCLUDED_GR_FOSPHOR_QT_SINK_C_IMPL_H */

