/* -*- c++ -*- */
/*
 * Copyright 2013-2021 Sylvain Munaut <tnt@246tNt.com>
 *
 * This file is part of gr-fosphor
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QGLWidget>

class QThread;

namespace gr {
  namespace fosphor {

    class qt_sink_c_impl;

    class QGLSurface : public ::QGLWidget
    {
      Q_OBJECT

      qt_sink_c_impl *d_block;
      QThread *d_gui_thread;

     protected:
      void hideEvent(QHideEvent *he);
      void showEvent(QShowEvent *he);
      void paintEvent(QPaintEvent *pe);
      void resizeEvent(QResizeEvent *re);
      void keyPressEvent(QKeyEvent *ke);
      void mousePressEvent(QMouseEvent *me);

     private slots:
      void giveContext(QThread *thread);

     public:
      QGLSurface(QWidget *parent, qt_sink_c_impl *d_block);

      void grabContext();
      void releaseContext();
    };

  } // namespace fosphor
} // namespace gr
