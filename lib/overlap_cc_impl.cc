/* -*- c++ -*- */
/*
 * Copyright 2013-2021 Sylvain Munaut <tnt@246tNt.com>
 *
 * This file is part of gr-fosphor
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdio.h>

#include <gnuradio/io_signature.h>

#include "overlap_cc_impl.h"


namespace gr {
  namespace fosphor {

overlap_cc::sptr
overlap_cc::make(int wlen, int overlap)
{
	return gnuradio::get_initial_sptr(
		new overlap_cc_impl(wlen, overlap)
	);
}

overlap_cc_impl::overlap_cc_impl(int wlen, int overlap)
  : gr::sync_interpolator("overlap_cc",
                          gr::io_signature::make(1, 1, sizeof(gr_complex)),
                          gr::io_signature::make(1, 1, sizeof(gr_complex)),
                          overlap),
    d_wlen(wlen)
{
	this->set_overlap_ratio(overlap);
}

overlap_cc_impl::~overlap_cc_impl()
{
}

void
overlap_cc_impl::set_overlap_ratio(const int overlap)
{
	/* Save the new ratio */
	this->d_overlap = overlap;

	/* Adapt the interpolation factor accordingly */
	this->set_interpolation(overlap);

	/* Always keep an history of a full window to properly
	 * support overlap ratio changes */
	this->set_history(this->d_wlen);

	/* Always output entire windows */
	this->set_output_multiple(this->d_wlen);
}

int
overlap_cc_impl::work(
	int noutput_items,
	gr_vector_const_void_star &input_items,
	gr_vector_void_star &output_items)
{
	const gr_complex *in = (const gr_complex *) input_items[0];
	gr_complex *out = (gr_complex *) output_items[0];
	int ii, oi;

	for (ii=0,oi=0; oi<noutput_items; ii+=this->d_wlen/this->d_overlap,oi+=this->d_wlen) {
		memcpy(&out[oi], &in[ii], this->d_wlen * sizeof(gr_complex));
	}

	return noutput_items;
}

  } /* namespace fosphor */
} /* namespace gr */
