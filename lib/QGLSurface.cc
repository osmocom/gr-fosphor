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

#include <QtEvents>
#include "QGLSurface.h"
#include "qt_sink_c_impl.h"

#include <stdio.h>

namespace gr {
  namespace fosphor {

QGLSurface::QGLSurface(QWidget *parent, qt_sink_c_impl *block)
  : QGLWidget(parent), d_block(block)
{
	this->doneCurrent();
	this->setFocusPolicy(Qt::StrongFocus);
}

void
QGLSurface::paintEvent(QPaintEvent *pe)
{
	/* Don't do anything */

	/*
	 * The default implementation calls makeCurrent but here we want
	 * _other_ threads to be current, so we need a dummy empty impl
	 * for the paintEvent
	 */
}

void
QGLSurface::resizeEvent(QResizeEvent *re)
{
	/*
	 * The default implementation calls makeCurrent but here we want
	 * _other_ threads to be current, so don't do that !
	 */

	/* Call back to main block */
	this->d_block->cb_reshape(re->size().width(), re->size().height());
}

void
QGLSurface::keyPressEvent(QKeyEvent *ke)
{
	switch (ke->key()) {
	case Qt::Key_Up:
		this->d_block->execute_ui_action(qt_sink_c_impl::REF_DOWN);
		break;
	case Qt::Key_Down:
		this->d_block->execute_ui_action(qt_sink_c_impl::REF_UP);
		break;
	case Qt::Key_Left:
		this->d_block->execute_ui_action(qt_sink_c_impl::DB_PER_DIV_DOWN);
		break;
	case Qt::Key_Right:
		this->d_block->execute_ui_action(qt_sink_c_impl::DB_PER_DIV_UP);
		break;
	case Qt::Key_Z:
		this->d_block->execute_ui_action(qt_sink_c_impl::ZOOM_TOGGLE);
		break;
	case Qt::Key_W:
		this->d_block->execute_ui_action(qt_sink_c_impl::ZOOM_WIDTH_UP);
		break;
	case Qt::Key_S:
		this->d_block->execute_ui_action(qt_sink_c_impl::ZOOM_WIDTH_DOWN);
		break;
	case Qt::Key_D:
		this->d_block->execute_ui_action(qt_sink_c_impl::ZOOM_CENTER_UP);
		break;
	case Qt::Key_A:
		this->d_block->execute_ui_action(qt_sink_c_impl::ZOOM_CENTER_DOWN);
		break;
	case Qt::Key_Q:
		this->d_block->execute_ui_action(qt_sink_c_impl::RATIO_UP);
		break;
	case Qt::Key_E:
		this->d_block->execute_ui_action(qt_sink_c_impl::RATIO_DOWN);
		break;
	}
}

  } /* namespace fosphor */
} /* namespace gr */
