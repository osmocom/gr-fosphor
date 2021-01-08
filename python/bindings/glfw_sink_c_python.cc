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

#include <gnuradio/fosphor/glfw_sink_c.h>

#define D(...) ""

void bind_glfw_sink_c(py::module& m)
{
	using glfw_sink_c = gr::fosphor::glfw_sink_c;

	py::class_<glfw_sink_c,
		gr::fosphor::base_sink_c,
		gr::sync_block,
		gr::block,
		gr::basic_block,
		std::shared_ptr<glfw_sink_c>>(m, "glfw_sink_c", D(glfw_sink_c))

		.def(py::init(&glfw_sink_c::make),
			D(glfw_sink_c,make)
		)

		;
}
