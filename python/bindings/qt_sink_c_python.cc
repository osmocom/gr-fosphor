/*
 * Copyright 2013-2021 Sylvain Munaut <tnt@246tNt.com>
 *
 * This file is part of gr-fosphor
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <QWidget>

#include <gnuradio/fosphor/qt_sink_c.h>

#define D(...) ""

void bind_qt_sink_c(py::module& m)
{
	using qt_sink_c = gr::fosphor::qt_sink_c;

	py::class_<qt_sink_c,
		gr::fosphor::base_sink_c,
		gr::sync_block,
		gr::block,
		gr::basic_block,
		std::shared_ptr<qt_sink_c>>(m, "qt_sink_c", D(qt_sink_c))

		.def(py::init(&qt_sink_c::make),
			py::arg("parent") = nullptr,
			D(qt_sink_c,make)
		)

		.def("exec_",
			&qt_sink_c::exec_,
			D(qt_sink_c, exec_)
		)

		.def("qwidget",
			&qt_sink_c::qwidget,
			D(qt_sink_c, qwidget)
		)

		//.def("pyqwidget",
		//	&qt_sink_c::pyqwidget,
		//	D(qt_sink_c, pyqwidget)
		//)
		// For the sip conversion to python to work, the widget object
		// needs to be explicitly converted to long long.
		.def("pyqwidget",
			[](std::shared_ptr<qt_sink_c> p) {
				return PyLong_AsLongLong(p->pyqwidget());
				},
			D(qt_sink_c, pyqwidget)
		)

		;
}
