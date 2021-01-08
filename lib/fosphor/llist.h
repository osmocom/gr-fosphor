/*
 * llist.h
 *
 * Simple double-linked list. Interface similar to the linux kernel
 * one, but heavily simplified and rewritten to compile on other compilers
 * than GCC.
 *
 * Copyright (C) 2013-2021 Sylvain Munaut
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

/*! \defgroup llist
 *  @{
 */

/*! \file llist.h
 *  \brief Simple double-linked list
 */

#include <stddef.h>


struct llist_head {
	struct llist_head *next, *prev;
};

#define LLIST_HEAD_INIT(name) { &(name), &(name) }

#define LLIST_HEAD(name) \
	struct llist_head name = LLIST_HEAD_INIT(name)

/**
 * \brief Add a new entry after the specified head
 * \param[in] new  new entry to be added
 * \param[in] head llist head to add it after
 */
static inline void llist_add(struct llist_head *_new, struct llist_head *head)
{
	head->next->prev = _new;
	_new->next = head->next;
	_new->prev = head;
	head->next = _new;
}

/**
 * \brief Deletes entry from llist
 * \param[in] entry the element to delete from the llist
 */
static inline void llist_del(struct llist_head *entry)
{
	entry->next->prev = entry->prev;
	entry->prev->next = entry->next;
	entry->next = (struct llist_head *)0;
	entry->prev = (struct llist_head *)0;
}

/**
 * \brief Get the struct for this entry
 * \param[in] ptr    the &struct llist_head pointer
 * \param[in] type   the type of the struct this is embedded in
 * \param[in] member the name of the llist_struct within the struct
 * \returns The struct for this entry
 */
#define llist_entry(ptr, type, member) \
        ((type *)( (char *)(ptr) - offsetof(type, member) ))

/**
 * \brief Iterate over llist of given type
 * \param[in]  type    the type of the loop counter
 * \param[out] pos     the type * to use as a loop counter
 * \param[in]  head    the head for your llist
 * \param[in]  member  the name of the llist_struct within the struct
 */
#define llist_for_each_entry(type, pos, head, member)				\
	for (pos = llist_entry((head)->next, type, member);	\
	     &pos->member != (head); 					\
	     pos = llist_entry(pos->member.next, type, member))

/*! @} */
