/*
 * fosphor.h
 *
 * Main fosphor entry point
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

#ifndef __FOSPHOR_FOSPHOR_H__
#define __FOSPHOR_FOSPHOR_H__

/*! \defgroup fosphor
 *  @{
 */

/*! \file fosphor.h
 *  \brief Main fosphor entry point
 */

struct fosphor;

struct fosphor *fosphor_init(void);
void fosphor_release(struct fosphor *self);

int  fosphor_process(struct fosphor *self, void *samples, int len);
void fosphor_draw(struct fosphor *self, int w, int h);

void fosphor_set_power_range(struct fosphor *self, int db_ref, int db_per_div);

/*! @} */

#endif /* __FOSPHOR_FOSPHOR_H__ */
