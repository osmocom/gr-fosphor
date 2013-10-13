/* -*- c++ -*- */
/*
 * Copyright 2013 Sylvain Munaut <tnt@246tNt.com>
 * Copyright 2013 Dimitri Stolnikov <horiz0n@gmx.net>
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

#include <gnuradio/fosphor/glfw_sink_c.h>

#include <GLFW/glfw3.h>

#include "fifo.h"

extern "C" {
#include "fosphor/fosphor.h"
}


namespace gr {
  namespace fosphor {

glfw_sink_c::sptr
glfw_sink_c::make()
{
	return gnuradio::get_initial_sptr(new glfw_sink_c());
}

const int glfw_sink_c::k_db_per_div[] = {1, 2, 5, 10, 20};

glfw_sink_c::glfw_sink_c()
  : gr::sync_block("glfw_sink_c",
                   gr::io_signature::make(1, 1, sizeof(gr_complex)),
                   gr::io_signature::make(0, 0, 0)),
    d_db_ref(0), d_db_per_div_idx(3)
{
	/* Init FIFO */
	this->d_fifo = new fifo(1024 * 1024);

	/* Start worker */
	this->d_active = true;
	this->d_worker = gr::thread::thread(_worker, this);
}

glfw_sink_c::~glfw_sink_c()
{
	this->d_active = false;
	this->d_worker.join();

	delete this->d_fifo;
}

int
glfw_sink_c::work(
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


void glfw_sink_c::worker()
{
	GLFWwindow *wnd;

	/* Init GLFW */
	glfwInit();

	/* Create window */
	wnd = glfwCreateWindow(1024, 1024, "fosphor", NULL, NULL);
	if (!wnd)
		return;

	this->d_window = wnd;

        glfwMakeContextCurrent(wnd);
	glfwSetWindowUserPointer(wnd, this);

	/* Setup callbacks */
	glfwSetFramebufferSizeCallback(wnd, _glfw_cb_reshape);
	glfwSetKeyCallback(wnd, _glfw_cb_key);

	/* Force first reshape */
	this->glfw_cb_reshape(-1, -1);

	/* Init fosphor */
	this->d_fosphor = fosphor_init();
	if (!this->d_fosphor)
		return;

	fosphor_set_range(this->d_fosphor,
		this->d_db_ref,
		this->k_db_per_div[this->d_db_per_div_idx]
	);

	/* Main loop */
	while (!glfwWindowShouldClose(wnd) && this->d_active)
	{
		this->glfw_render();
		glfwPollEvents();
	}

	/* Cleanup fosphor */
	fosphor_release(this->d_fosphor);

	/* And GLFW */
	glfwDestroyWindow(wnd);
	glfwTerminate();
}

void glfw_sink_c::_worker(glfw_sink_c *obj)
{
        obj->worker();
}


void
glfw_sink_c::glfw_render(void)
{
	/* Clear everything */
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClear(GL_COLOR_BUFFER_BIT);

	/* Flush fifo */
	while (this->d_fifo->used() > 128*1024) {
		gr_complex *data = this->d_fifo->read_peek(128*1024, false);
		fosphor_process(this->d_fosphor, data, 128*1024);
		this->d_fifo->read_discard(128*1024);
	}

	/* Draw */
	fosphor_draw(this->d_fosphor, this->d_width, this->d_height);

	/* Done, swap buffer */
	glfwSwapBuffers(this->d_window);
}

void
glfw_sink_c::glfw_cb_reshape(int w, int h)
{
	if (w < 0 || h < 0)
		glfwGetFramebufferSize(this->d_window, &w, &h);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, (double)w, 0.0, (double)h, -1.0, 1.0);

	glViewport(0, 0, w, h);

	this->d_width = w;
	this->d_height = h;
}

void
glfw_sink_c::glfw_cb_key(int key, int scancode, int action, int mods)
{
	if (action != GLFW_PRESS)
		return;

	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		exit(0);
		break;

	case GLFW_KEY_UP:
		this->d_db_ref -= k_db_per_div[this->d_db_per_div_idx];
		break;

	case GLFW_KEY_DOWN:
		this->d_db_ref += k_db_per_div[this->d_db_per_div_idx];
		break;

	case GLFW_KEY_LEFT:
		if (this->d_db_per_div_idx > 0)
			this->d_db_per_div_idx--;
		break;

	case GLFW_KEY_RIGHT:
		if (this->d_db_per_div_idx < 4)
			this->d_db_per_div_idx++;
		break;
	}

	fosphor_set_range(this->d_fosphor,
		this->d_db_ref,
		this->k_db_per_div[this->d_db_per_div_idx]
	);
}

void
glfw_sink_c::_glfw_cb_reshape(GLFWwindow *wnd, int w, int h)
{
	glfw_sink_c *sink = (glfw_sink_c *) glfwGetWindowUserPointer(wnd);
	sink->glfw_cb_reshape(w, h);
}

void
glfw_sink_c::_glfw_cb_key(GLFWwindow *wnd, int key, int scancode, int action, int mods)
{
	glfw_sink_c *sink = (glfw_sink_c *) glfwGetWindowUserPointer(wnd);
	sink->glfw_cb_key(key, scancode, action, mods);
}

  } /* namespace fosphor */
} /* namespace gr */
