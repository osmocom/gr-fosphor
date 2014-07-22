/*
 * resource_internal.h
 *
 * Internal structures for the resource management
 *
 * Copyright (C) 2013-2014 Sylvain Munaut
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __RESOURCE_INTERNAL_H__
#define __RESOURCE_INTERNAL_H__

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

#endif /* __RESOURCE_INTERNAL_H__ */
