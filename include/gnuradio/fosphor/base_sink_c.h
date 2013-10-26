/* -*- c++ -*- */
/*
 * Copyright 2013 Sylvain Munaut <tnt@246tNt.com>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */


#ifndef INCLUDED_GR_FOSPHOR_BASE_SINK_C_H
#define INCLUDED_GR_FOSPHOR_BASE_SINK_C_H

#include <gnuradio/fosphor/api.h>

#include <gnuradio/sync_block.h>

namespace gr {
  namespace fosphor {

    /*!
     * \brief Base fosphor sink API interface
     * \ingroup fosphor
     */
    class GR_FOSPHOR_API base_sink_c : public gr::sync_block
    {
     protected:
      base_sink_c(const char *name = NULL);

     public:

      enum ui_action_t {
        DB_PER_DIV_UP,
        DB_PER_DIV_DOWN,
        REF_UP,
        REF_DOWN,
      };

      virtual void execute_ui_action(enum ui_action_t action) = 0;
    };

  } // namespace fosphor
} // namespace gr

#endif /* INCLUDED_GR_FOSPHOR_BASE_SINK_C_H */

