/* -*- c++ -*- */
/*
 * Copyright 2013-2021 Sylvain Munaut <tnt@246tNt.com>
 *
 * This file is part of gr-fosphor
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef ENABLE_PYTHON
# include <Python.h>
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "qt_sink_c_impl.h"

#include <QApplication>
#include <QWidget>
#include <QGLWidget>

#include "QGLSurface.h"


namespace gr {
  namespace fosphor {

qt_sink_c::sptr
qt_sink_c::make(QWidget *parent)
{
	return gnuradio::get_initial_sptr(new qt_sink_c_impl(parent));
}

qt_sink_c_impl::qt_sink_c_impl(QWidget *parent)
  : base_sink_c("qt_sink_c")
{
	/* QT stuff */
	if(qApp != NULL) {
		d_qApplication = qApp;
	}
	else {
		int argc=0;
		char **argv = NULL;
		d_qApplication = new QApplication(argc, argv);
	}

	this->d_gui = new QGLSurface(parent, this);
}


void
qt_sink_c_impl::glctx_init()
{
	this->d_gui->grabContext();
	this->d_gui->setFocus();
}

void
qt_sink_c_impl::glctx_swap()
{
	this->d_gui->swapBuffers();
}

void
qt_sink_c_impl::glctx_poll()
{
	/* Nothing to do */
}

void
qt_sink_c_impl::glctx_fini()
{
	this->d_gui->releaseContext();
}

void
qt_sink_c_impl::glctx_update()
{
	this->d_gui->makeCurrent();
}


void
qt_sink_c_impl::exec_()
{
	d_qApplication->exec();
}

QWidget*
qt_sink_c_impl::qwidget()
{
	return dynamic_cast<QWidget*>(this->d_gui);
}


#ifdef ENABLE_PYTHON
PyObject*
qt_sink_c_impl::pyqwidget()
{
	PyObject *w = PyLong_FromVoidPtr((void*)dynamic_cast<QWidget*>(this->d_gui));
	PyObject *retarg = Py_BuildValue("N", w);
	return retarg;
}
#else
void*
qt_sink_c_impl::pyqwidget()
{
	return NULL;
}
#endif

  } /* namespace fosphor */
} /* namespace gr */
