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

#include <GLFW/glfw3.h>

#include "glfw_sink_c_impl.h"


namespace gr {
  namespace fosphor {

glfw_sink_c::sptr
glfw_sink_c::make()
{
	return gnuradio::get_initial_sptr(new glfw_sink_c_impl());
}

glfw_sink_c_impl::glfw_sink_c_impl()
  : base_sink_c("glfw_sink_c")
{
	/* Nothing to do but super call */
}


void
glfw_sink_c_impl::glfw_cb_reshape(int w, int h)
{
	if (w < 0 || h < 0)
		glfwGetFramebufferSize(this->d_window, &w, &h);

	this->cb_reshape(w, h);
}

void
glfw_sink_c_impl::glfw_cb_key(int key, int scancode, int action, int mods)
{
	if (action != GLFW_PRESS)
		return;

	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		exit(0);
		break;

	case GLFW_KEY_UP:
		this->base_sink_c_impl::execute_ui_action(REF_DOWN);
		break;

	case GLFW_KEY_DOWN:
		this->base_sink_c_impl::execute_ui_action(REF_UP);
		break;

	case GLFW_KEY_LEFT:
		this->base_sink_c_impl::execute_ui_action(DB_PER_DIV_DOWN);
		break;

	case GLFW_KEY_RIGHT:
		this->base_sink_c_impl::execute_ui_action(DB_PER_DIV_UP);
		break;
	}
}

void
glfw_sink_c_impl::_glfw_cb_reshape(GLFWwindow *wnd, int w, int h)
{
	glfw_sink_c_impl *sink = (glfw_sink_c_impl *) glfwGetWindowUserPointer(wnd);
	sink->glfw_cb_reshape(w, h);
}

void
glfw_sink_c_impl::_glfw_cb_key(GLFWwindow *wnd, int key, int scancode, int action, int mods)
{
	glfw_sink_c_impl *sink = (glfw_sink_c_impl *) glfwGetWindowUserPointer(wnd);
	sink->glfw_cb_key(key, scancode, action, mods);
}


void
glfw_sink_c_impl::glctx_init()
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
}

void
glfw_sink_c_impl::glctx_swap()
{
	glfwSwapBuffers(this->d_window);
}

void
glfw_sink_c_impl::glctx_poll()
{
	glfwPollEvents();
}

void
glfw_sink_c_impl::glctx_fini()
{
	glfwDestroyWindow(this->d_window);
	glfwTerminate();
}

void
glfw_sink_c_impl::glctx_update()
{
	/* Nothing to do for GLFW */
}


  } /* namespace fosphor */
} /* namespace gr */
