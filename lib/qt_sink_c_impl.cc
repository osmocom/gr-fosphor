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
  : base_sink_c("glfw_sink_c")
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
	this->d_gui->makeCurrent();
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
	this->d_gui->doneCurrent();
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
