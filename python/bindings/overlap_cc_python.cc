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

#include <gnuradio/fosphor/overlap_cc.h>

#define D(...) ""

void bind_overlap_cc(py::module& m)
{
	using overlap_cc = gr::fosphor::overlap_cc;

	py::class_<overlap_cc,
		gr::sync_block,
		gr::block,
		gr::basic_block,
		std::shared_ptr<overlap_cc>>(m, "overlap_cc", D(overlap_cc))

		.def(py::init(&overlap_cc::make),
			py::arg("wlen"),
			py::arg("overlap"),
			D(overlap_cc,make)
		)

		.def("set_overlap_ration",
			&overlap_cc::set_overlap_ratio,
			py::arg("overlap"),
			D(overlap_cc, set_overlap_ratio)
		)

		;
}
