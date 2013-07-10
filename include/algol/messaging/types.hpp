/* libalgol - a collection of plug-ins for developing back-end C++ web tools
 * Copyright (c) 2013 Algol Labs, LLC.
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

#ifndef H_ALGOL_MESSAGING_TYPES_H
#define H_ALGOL_MESSAGING_TYPES_H

#include "algol/algol.hpp"
#include <amqp.h>
#include <amqp_framing.h>

namespace algol {

  typedef string_t channel_id_t;
  typedef string_t queue_id_t;

  enum class comm_rc : unsigned char {
    unassigned = 0, // don't add anything before this

    success, /** communication was successful */
    invalid_message, /** messages are invalid if they have no payload */
    link_unavailable, /** a connection error with the communication platform */

    sanity_check // don't add anything after this
  };

} // end of namespace algol

#endif
