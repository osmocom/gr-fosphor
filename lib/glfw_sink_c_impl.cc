/* -*- c++ -*- */
/*
 * Copyright 2013-2014 Sylvain Munaut <tnt@246tNt.com>
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
	this->cb_visibility(true);
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
		this->execute_ui_action(REF_DOWN);
		break;

	case GLFW_KEY_DOWN:
		this->execute_ui_action(REF_UP);
		break;

	case GLFW_KEY_LEFT:
		this->execute_ui_action(DB_PER_DIV_DOWN);
		break;

	case GLFW_KEY_RIGHT:
		this->execute_ui_action(DB_PER_DIV_UP);
		break;

	case GLFW_KEY_Z:
		this->execute_ui_action(ZOOM_TOGGLE);
		break;

	case GLFW_KEY_W:
		this->execute_ui_action(ZOOM_WIDTH_UP);
		break;

	case GLFW_KEY_S:
		this->execute_ui_action(ZOOM_WIDTH_DOWN);
		break;

	case GLFW_KEY_D:
		this->execute_ui_action(ZOOM_CENTER_UP);
		break;

	case GLFW_KEY_A:
		this->execute_ui_action(ZOOM_CENTER_DOWN);
		break;

	case GLFW_KEY_Q:
		this->execute_ui_action(RATIO_UP);
		break;

	case GLFW_KEY_E:
		this->execute_ui_action(RATIO_DOWN);
		break;

	case GLFW_KEY_SPACE:
		this->execute_ui_action(FREEZE_TOGGLE);
		break;
	}
}

void
glfw_sink_c_impl::glfw_cb_mouse(int btn, int action, int mods)
{
	int x, y, w, h;
	double xd, yd;

	if (action != GLFW_PRESS)
		return;

	/* Get cursor position */
	glfwGetFramebufferSize(this->d_window, &w, &h);
	glfwGetCursorPos(this->d_window, &xd, &yd);

	x = floor(xd);
	y = h - floor(yd) - 1;

	/* Report upstream */
	this->execute_mouse_action(glfw_sink_c_impl::CLICK, x, y);
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
glfw_sink_c_impl::_glfw_cb_mouse(GLFWwindow *wnd, int btn, int action, int mods)
{
	glfw_sink_c_impl *sink = (glfw_sink_c_impl *) glfwGetWindowUserPointer(wnd);
	sink->glfw_cb_mouse(btn, action, mods);
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
	glfwSetMouseButtonCallback(wnd, _glfw_cb_mouse);

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
