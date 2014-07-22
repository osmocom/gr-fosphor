/* -*- c++ -*- */
/*
 * Copyright 2013-2014 Sylvain Munaut <tnt@246tNt.com>
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

#include <gnuradio/thread/thread.h>

#include "fifo.h"

namespace gr {
  namespace fosphor {

fifo::fifo(int length) :
	d_len(length), d_rp(0), d_wp(0)
{
	this->d_buf = new gr_complex[this->d_len];
}

fifo::~fifo()
{
	delete[] this->d_buf;
}

int
fifo::free()
{
	return (this->d_len - 1) - this->used();
}

int
fifo::used()
{
	return (this->d_wp - this->d_rp) & (this->d_len - 1);
}

int
fifo::write_max_size()
{
	return this->d_len - this->d_wp;
}

gr_complex *
fifo::write_prepare(int size, bool wait)
{
	gr::thread::scoped_lock lock(this->d_mutex);

	if (!wait && (this->free() < size))
		return NULL;

	while (this->free() < size)
		this->d_cond_full.wait(lock);

	return &this->d_buf[this->d_wp];
}

void
fifo::write_commit(int size)
{
	gr::thread::scoped_lock lock(this->d_mutex);

	this->d_wp = (this->d_wp + size) & (this->d_len - 1);

	this->d_cond_empty.notify_one();
}

int
fifo::read_max_size()
{
	return this->d_len - this->d_rp;
}

gr_complex *
fifo::read_peek(int size, bool wait)
{
	gr::thread::scoped_lock lock(this->d_mutex);

	if (!wait && (this->used() < size))
		return NULL;

	while (this->used() < size)
		this->d_cond_empty.wait(lock);

	return &this->d_buf[this->d_rp];
}

void
fifo::read_discard(int size)
{
	gr::thread::scoped_lock lock(this->d_mutex);

	this->d_rp = (this->d_rp + size) & (this->d_len - 1);

	this->d_cond_full.notify_one();
}

  } /* namespace fosphor */
} /* namespace gr */
