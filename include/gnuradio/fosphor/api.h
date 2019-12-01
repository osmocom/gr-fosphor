/*
 * Copyright 2011 Free Software Foundation, Inc.
 *
 * This file is part of gr-fosphor
 *
 * gr-fosphor is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * gr-fosphor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with gr-fosphor; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_GR_FOSPHOR_API_H
#define INCLUDED_GR_FOSPHOR_API_H

#include <gnuradio/attributes.h>

#ifdef gnuradio_fosphor_EXPORTS
#  define GR_FOSPHOR_API __GR_ATTR_EXPORT
#else
#  define GR_FOSPHOR_API __GR_ATTR_IMPORT
#endif

#endif /* INCLUDED_GR_FOSPHOR_API_H */
