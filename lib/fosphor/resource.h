/*
 * resource.h
 *
 * Resource management header
 *
 * Copyright (C) 2013-2021 Sylvain Munaut
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

/*! \defgroup resource
 *  @{
 */

/*! \file resource.h
 *  \brief Resource management header
 */

const void *resource_get(const char *name, int *len);
void        resource_put(const void *r);

/*! @} */
