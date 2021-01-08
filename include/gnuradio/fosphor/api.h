/*
 * Copyright 2011 Free Software Foundation, Inc.
 * Copyright 2013-2021 Sylvain Munaut <tnt@246tNt.com>
 *
 * This file is part of gr-fosphor
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <gnuradio/attributes.h>

#ifdef gnuradio_fosphor_EXPORTS
#  define GR_FOSPHOR_API __GR_ATTR_EXPORT
#else
#  define GR_FOSPHOR_API __GR_ATTR_IMPORT
#endif
