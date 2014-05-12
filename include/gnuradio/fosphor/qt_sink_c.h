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


#ifndef INCLUDED_GR_FOSPHOR_QT_SINK_C_H
#define INCLUDED_GR_FOSPHOR_QT_SINK_C_H

#include <gnuradio/fosphor/api.h>
#include <gnuradio/fosphor/base_sink_c.h>

#include <gnuradio/sync_block.h>

class QApplication;
class QWidget;

namespace gr {
  namespace fosphor {

    /*!
     * \brief Qt version of fosphor sink
     * \ingroup fosphor
     */
    class GR_FOSPHOR_API qt_sink_c : virtual public base_sink_c
    {
     public:
      typedef boost::shared_ptr<qt_sink_c> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of fosphor::qt_sink_c.
       *
       * To avoid accidental use of raw pointers, fosphor::qt_sink_c's
       * constructor is in a private implementation
       * class. fosphor::qt_sink_c::make is the public interface for
       * creating new instances.
       */
      static sptr make(QWidget *parent=NULL);

      virtual void exec_() = 0;
      virtual QWidget* qwidget() = 0;

#if defined(PY_VERSION) || defined(SWIGPYTHON)
      virtual PyObject* pyqwidget() = 0;
#else
      virtual void* pyqwidget() = 0;
#endif

      QApplication *d_qApplication;
    };

  } // namespace fosphor
} // namespace gr

#endif /* INCLUDED_GR_FOSPHOR_QT_SINK_C_H */

