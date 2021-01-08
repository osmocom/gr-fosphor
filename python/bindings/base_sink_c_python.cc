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

#include <gnuradio/fosphor/base_sink_c.h>

#define D(...) ""

void bind_base_sink_c(py::module& m)
{
	using base_sink_c = gr::fosphor::base_sink_c;

	py::class_<base_sink_c,
		gr::sync_block,
		gr::block,
		gr::basic_block,
		std::shared_ptr<base_sink_c>> sink_class (m, "base_sink_c", D(base_sink_c));


	py::enum_<base_sink_c::ui_action_t>(sink_class, "ui_action")
	.value("DB_PER_DIV_UP",    base_sink_c::DB_PER_DIV_UP)
	.value("DB_PER_DIV_DOWN",  base_sink_c::DB_PER_DIV_DOWN)
	.value("REF_UP",           base_sink_c::REF_UP)
	.value("REF_DOWN",         base_sink_c::REF_DOWN)
	.value("ZOOM_TOGGLE",      base_sink_c::ZOOM_TOGGLE)
	.value("ZOOM_WIDTH_UP",    base_sink_c::ZOOM_WIDTH_UP)
	.value("ZOOM_WIDTH_DOWN",  base_sink_c::ZOOM_WIDTH_DOWN)
	.value("ZOOM_CENTER_UP",   base_sink_c::ZOOM_CENTER_UP)
	.value("ZOOM_CENTER_DOWN", base_sink_c::ZOOM_CENTER_DOWN)
	.value("RATIO_UP",         base_sink_c::RATIO_UP)
	.value("RATIO_DOWN",       base_sink_c::RATIO_DOWN)
	.value("FREEZE_TOGGLE",    base_sink_c::FREEZE_TOGGLE)
        .export_values();

	py::enum_<base_sink_c::mouse_action_t>(sink_class, "mouse_action")
	.value("CLICK",           base_sink_c::CLICK)
        .export_values();

	py::implicitly_convertible<int, base_sink_c::ui_action_t>();
	py::implicitly_convertible<int, base_sink_c::mouse_action_t>();

	sink_class
		.def("execute_ui_action",
			&base_sink_c::execute_ui_action,
			py::arg("action"),
			D(base_sink_c,execute_ui_action)
		)

		.def("execute_mouse_action",
			&base_sink_c::execute_mouse_action,
			py::arg("action"),
			py::arg("x"),
			py::arg("y"),
			D(base_sink_c,execute_mouse_action)
		)

		.def("set_frequency_range",
			&base_sink_c::set_frequency_range,
			py::arg("center"),
			py::arg("span"),
			D(base_sink_c,set_frequency_range)
		)

		.def("set_frequency_center",
			&base_sink_c::set_frequency_center,
			py::arg("center"),
			D(base_sink_c,set_frequency_center)
		)

		.def("set_frequency_span",
			&base_sink_c::set_frequency_span,
			py::arg("span"),
			D(base_sink_c,set_frequency_span)
		)

		.def("set_fft_window",
			&base_sink_c::set_fft_window,
			py::arg("win"),
			D(base_sink_c,set_fft_window)
		)

		;
}
