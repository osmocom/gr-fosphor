/*
 * resource_internal.h
 *
 * Internal structures for the resource management
 *
 * Copyright (C) 2013-2021 Sylvain Munaut
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

/*! \addtogroup resource
 *  @{
 */

/*! \file resource_internal.h
 *  \brief Internal header for the resource management
 */

#include "llist.h"

/*! \brief Internal structure describing a packed resource */
struct resource_pack
{
	const char *name;	/*!< \brief Name of the resource */
	const void *data;	/*!< \brief Data (with added final \0) */
	unsigned int len;	/*!< \brief Original length (in bytes) of data */
};

struct resource_cache
{
	struct llist_head head;	/*< \brief Linked list head for cache */

	const char *name;	/*< \brief Name of the resource */
	const void *data;	/*< \brief Data pointer given to user */
	unsigned int len;	/*< \brief riginal length (in bytes) of data */

	int refcnt;		/*< \brief Reference counter */
	int flags;		/*< \brief Flags */

#define RES_FLAG_MALLOCED	(1 << 0)

	char extra[0];		/*< \brief Extra data for whatever ... */
};

/*! @} */
