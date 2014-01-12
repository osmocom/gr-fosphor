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

#include <Python.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "wx_core_sink_c_impl.h"


namespace gr {
  namespace fosphor {

wx_core_sink_c::sptr
wx_core_sink_c::make(PyObject *cb_init, PyObject *cb_fini,
                     PyObject *cb_swap, PyObject *cb_update)
{
	return gnuradio::get_initial_sptr(
		new wx_core_sink_c_impl(cb_init, cb_fini, cb_swap, cb_update)
	);
}

wx_core_sink_c_impl::wx_core_sink_c_impl(PyObject *cb_init, PyObject *cb_fini,
                                         PyObject *cb_swap, PyObject *cb_update)
  : base_sink_c("wx_core_sink_c"),
    d_cb_init(cb_init), d_cb_fini(cb_fini),
    d_cb_swap(cb_swap), d_cb_update(cb_update)
{
	/* Make sure we keep reference to callbacks */
	Py_INCREF(this->d_cb_init);
	Py_INCREF(this->d_cb_fini);
	Py_INCREF(this->d_cb_swap);
	Py_INCREF(this->d_cb_update);
}

wx_core_sink_c_impl::~wx_core_sink_c_impl()
{
	/* Release callbacks */
	Py_DECREF(this->d_cb_init);
	Py_DECREF(this->d_cb_fini);
	Py_DECREF(this->d_cb_swap);
	Py_DECREF(this->d_cb_update);
}


void
wx_core_sink_c_impl::pycall(PyObject *cb)
{
	PyObject *arglist;
	PyObject *result;

	PyGILState_STATE gstate;
	gstate = PyGILState_Ensure();

	arglist = Py_BuildValue("()");
	result = PyEval_CallObject(cb, arglist);
	Py_DECREF(arglist);

	if (result != NULL)
		Py_DECREF(result);

	PyGILState_Release(gstate);
}


void
wx_core_sink_c_impl::glctx_init()
{
	this->pycall(this->d_cb_init);
}

void
wx_core_sink_c_impl::glctx_swap()
{
	this->pycall(this->d_cb_swap);
}

void
wx_core_sink_c_impl::glctx_poll()
{
	/* Nothing to do */
}

void
wx_core_sink_c_impl::glctx_fini()
{
	this->pycall(this->d_cb_fini);
}

void
wx_core_sink_c_impl::glctx_update()
{
	this->pycall(this->d_cb_update);
}


void
wx_core_sink_c_impl::pycb_reshape(int width, int height)
{
	this->cb_reshape(width, height);
}

  } /* namespace fosphor */
} /* namespace gr */
