/*
 * resource.h
 *
 * Resource management header
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

#ifndef __FOSPHOR_RESOURCE_H__
#define __FOSPHOR_RESOURCE_H__

/*! \defgroup resource
 *  @{
 */

/*! \file resource.h
 *  \brief Resource management header
 */

const void *resource_get(const char *name, int *len);
void        resource_put(const void *r);

/*! @} */

#endif /* __FOSPHOR_RESOURCE_H__ */
