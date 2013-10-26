/*
 * config.h
 *
 * Global fosphor configuration constants
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

#ifndef __FOSPHOR_CONFIG_H__
#define __FOSPHOR_CONFIG_H__

/*! \defgroup config
 *  @{
 */

/*! \file config.h
 *  \brief Global fosphor configuration constants
 */


#define FOSPHOR_FFT_LEN_LOG	10
#define FOSPHOR_FFT_LEN		(1<<FOSPHOR_FFT_LEN_LOG)
#define FOSPHOR_FFT_MAX_BATCH	128


/*! @} */

#endif /* __FOSPHOR_CONFIG_H__ */
