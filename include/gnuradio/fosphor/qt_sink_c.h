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
