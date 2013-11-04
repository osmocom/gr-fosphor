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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdio.h>

#include <gnuradio/io_signature.h>
#include <gnuradio/thread/thread.h>

#include "fifo.h"
#include "base_sink_c_impl.h"

extern "C" {
#include "fosphor/fosphor.h"
#include "fosphor/gl_platform.h"
}


namespace gr {
  namespace fosphor {

base_sink_c::base_sink_c(const char *name)
  : gr::sync_block(name,
                   gr::io_signature::make(1, 1, sizeof(gr_complex)),
                   gr::io_signature::make(0, 0, 0))
{
	/* Nothing to do but super call */
}


const int base_sink_c_impl::k_db_per_div[] = {1, 2, 5, 10, 20};


base_sink_c_impl::base_sink_c_impl()
  : d_db_ref(0), d_db_per_div_idx(3), d_active(false)
{
	/* Init FIFO */
	this->d_fifo = new fifo(2 * 1024 * 1024);
}

base_sink_c_impl::~base_sink_c_impl()
{
	delete this->d_fifo;
}


void base_sink_c_impl::worker()
{
	/* Init GL context */
	this->glctx_init();

	/* Init fosphor */
	this->d_fosphor = fosphor_init();
	if (!this->d_fosphor)
		return;

	fosphor_set_range(this->d_fosphor,
		this->d_db_ref,
		this->k_db_per_div[this->d_db_per_div_idx]
	);

	/* Main loop */
	while (this->d_active)
	{
		this->render();
		this->glctx_poll();
	}

	/* Cleanup fosphor */
	fosphor_release(this->d_fosphor);

	/* And GL context */
	this->glctx_fini();
}

void base_sink_c_impl::_worker(base_sink_c_impl *obj)
{
        obj->worker();
}


void
base_sink_c_impl::render(void)
{
	const int fft_len    = 1024;
	const int batch_mult = 16;
	const int batch_max  = 1024;
	const int max_iter   = 8;

	int i, tot_len;

	/* Handle pending reshape */
	if (this->d_reshaped)
	{
		this->d_reshaped = false;

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0, (double)this->d_width, 0.0, (double)this->d_height, -1.0, 1.0);

		glViewport(0, 0, this->d_width, this->d_height);
	}

	/* Clear everything */
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClear(GL_COLOR_BUFFER_BIT);

	tot_len = this->d_fifo->used();

	/* Process as much we can */
	for (i=0; i<max_iter && tot_len; i++)
	{
		gr_complex *data;
		int len;

		/* How much can we get from FIFO in one block */
		len = tot_len;
		if (len > this->d_fifo->read_max_size())
			len = this->d_fifo->read_max_size();

		/* Adapt to valid size for fosphor */
		len &= ~((batch_mult * fft_len) - 1);
		if (len > (batch_max * fft_len))
			len = batch_max * fft_len;

		/* Is it worth it ? */
		tot_len -= len;

		if (!len)
			break;

		/* Send to process */
		data = this->d_fifo->read_peek(len, false);
		fosphor_process(this->d_fosphor, data, len);

		/* Discard */
		this->d_fifo->read_discard(len);
	}

	/* Draw */
	fosphor_draw(this->d_fosphor, this->d_width, this->d_height);

	/* Done, swap buffer */
	this->glctx_swap();
}


void
base_sink_c_impl::cb_reshape(int width, int height)
{
	this->d_width    = width;
	this->d_height   = height;
	this->d_reshaped = true;
}


void
base_sink_c_impl::execute_ui_action(enum ui_action_t action)
{
	switch (action) {
	case DB_PER_DIV_UP:
		if (this->d_db_per_div_idx < 4)
			this->d_db_per_div_idx++;
		break;

	case DB_PER_DIV_DOWN:
		if (this->d_db_per_div_idx > 0)
			this->d_db_per_div_idx--;
		break;

	case REF_UP:
		this->d_db_ref += k_db_per_div[this->d_db_per_div_idx];
		break;

	case REF_DOWN:
		this->d_db_ref -= k_db_per_div[this->d_db_per_div_idx];
		break;
	}

	fosphor_set_range(this->d_fosphor,
		this->d_db_ref,
		this->k_db_per_div[this->d_db_per_div_idx]
	);
}


int
base_sink_c_impl::work(
	int noutput_items,
	gr_vector_const_void_star &input_items,
	gr_vector_void_star &output_items)
{
	const gr_complex *in = (const gr_complex *) input_items[0];
	gr_complex *dst;
	int l, mw;

	/* How much can we hope to write */
	l = noutput_items;
	mw = this->d_fifo->write_max_size();

	if (l > mw)
		l = mw;
	if (!l)
		return 0;

	/* Get a pointer */
	dst = this->d_fifo->write_prepare(l, true);
	if (!dst)
		return 0;

	/* Do the copy */
	memcpy(dst, in, sizeof(gr_complex) * l);
	this->d_fifo->write_commit(l);

	/* Report what we took */
	return l;
}

bool base_sink_c_impl::start()
{
	bool rv = base_sink_c::start();
	if (!this->d_active) {
		this->d_active = true;
		this->d_worker = gr::thread::thread(_worker, this);
	}
	return rv;
}

bool base_sink_c_impl::stop()
{
	bool rv = base_sink_c::stop();
	if (this->d_active) {
		this->d_active = false;
		this->d_worker.join();
	}
	return rv;
}

  } /* namespace fosphor */
} /* namespace gr */
