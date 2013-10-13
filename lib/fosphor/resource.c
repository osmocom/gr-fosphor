/*
 * resource_internal.h
 *
 * Resource management
 *
 * Copyright (C) 2013 Sylvain Munaut
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

/*! \addtogroup resource
 *  @{
 */

/*! \file resource.c
 *  \brief Resource management implementation
 */

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "resource.h"
#include "resource_internal.h"

#ifndef PATH_MAX
# define PATH_MAX 1024
#endif


/*! \brief Packed resources */
extern struct resource_pack __resources[];

/*! \brief Loaded resources */
static LLIST_HEAD(g_cache);


/* ------------------------------------------------------------------------ */
/* Loading                                                                  */
/* ------------------------------------------------------------------------ */


static FILE *
_rc_try_open(struct resource_cache *rc)
{
	FILE *fh;
	char p[PATH_MAX];
	char *env;

	/* Try locally */
	fh = fopen(rc->name, "rb");
	if (fh)
		return fh;

	/* Try environment path */
	env = getenv("RESOURCE_PATH");
	if (!env)
		return NULL;

	if ((strlen(env) + strlen(rc->name) + 2) > PATH_MAX)
		return NULL;

	sprintf(p, "%s/%s", env, rc->name);

	/* Retry full path */
	fh = fopen(p, "rb");

	return fh;
}

static int
_rc_load(struct resource_cache *rc)
{
	FILE *fh;

	/* Try to find file */
	fh = _rc_try_open(rc);

	if (fh) {
		char *data;
		long len;

		/* Get len */
		fseek(fh, 0, SEEK_END);
		len = ftell(fh);
		fseek(fh, 0, SEEK_SET);

		/* Get buffer with size for added \0 */
		data = malloc(len+1);
		if (!data)
			goto read_done;

		/* Fill it */
		if (fread(data, len, 1, fh) != 1) {
			free(data);
			goto read_done;
		}

		data[len] = '\x00';

		/* Store result */
		rc->data   = data;
		rc->len    = len;
		rc->flags |= RES_FLAG_MALLOCED;

read_done:
		/* Done */
		fclose(fh);
	}

	/* Search packed resource */
	if (!rc->data)
	{
		struct resource_pack *rp = __resources;

		while (rp->name) {
			if (!strcmp(rp->name, rc->name)) {
				rc->data = rp->data;
				rc->len = rp->len;
				break;
			}
			rp++;
		}
	}

	return rc->data ? 0 : -ENOENT;
}

static void
_rc_unload(struct resource_cache *rc)
{
	if (rc->flags & RES_FLAG_MALLOCED)
		free((void*)rc->data);
}


/* ------------------------------------------------------------------------ */
/* Internal cache                                                           */
/* ------------------------------------------------------------------------ */

static struct resource_cache *
_rc_alloc(const char *name)
{
	size_t l;
	struct resource_cache *rc;
	char *n;

	l = sizeof(struct resource_cache) + strlen(name) + 1;
	rc = malloc(l);
	if (!rc)
		return NULL;

	memset(rc, 0x00, l);

	n = &rc->extra[0];
	strcpy(n, name);
	rc->name = n;

	return rc;
}

static struct resource_cache *
_rc_find_by_name(const char *name)
{
	struct resource_cache *rc;

	llist_for_each_entry(struct resource_cache, rc, &g_cache, head) {
		if (!strcmp(rc->name, name))
			return rc;
	}

	return NULL;
}

static struct resource_cache *
_rc_find_by_data(const void *data)
{
	struct resource_cache *rc;

	llist_for_each_entry(struct resource_cache, rc, &g_cache, head) {
		if (rc->data == data)
			return rc;
	}

	return NULL;
}

static struct resource_cache *
_rc_get(struct resource_cache *rc)
{
	rc->refcnt++;
	return rc;
}

static struct resource_cache *
_rc_get_new(const char *name)
{
	struct resource_cache *rc;

	rc = _rc_alloc(name);
	if (!rc)
		return NULL;

	if (_rc_load(rc)) {
		free(rc);
		return NULL;
	}

	llist_add(&rc->head, &g_cache);

	return _rc_get(rc);
}

static struct resource_cache *
_rc_put(struct resource_cache *rc)
{
	assert(rc->refcnt > 0);

	rc->refcnt--;

	if (rc->refcnt == 0) {
		llist_del(&rc->head);
		_rc_unload(rc);
		free(rc);
		rc = NULL;
	}

	return rc;
}


/* ------------------------------------------------------------------------ */
/* Public API                                                               */
/* ------------------------------------------------------------------------ */

const void *
resource_get(const char *name, int *len)
{
	struct resource_cache *rc;

	/* Search the cache */
	rc = _rc_find_by_name(name);
	if (rc)
		goto done;

	/* No such luck ? Need a new entry */
	rc = _rc_get_new(name);
	if (!rc)
		return NULL;

done:
	if (rc && len)
		*len = rc->len;

	return rc ? rc->data : NULL;
}

void
resource_put(const void *r)
{
	struct resource_cache *rc;

	/* Search the cache */
	rc = _rc_find_by_data(r);
	if (!rc)
		return;

	/* We're done with it */
	_rc_put(rc);
}

/*! @} */
