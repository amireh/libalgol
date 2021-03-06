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

#ifndef H_ALGOL_HTTP_FORMATS_H
#define H_ALGOL_HTTP_FORMATS_H

#include <algol/algol.hpp>

namespace algol {
namespace http {

  enum {
    HTTP_FORMAT_NONE,
    HTTP_FORMAT_HTML,
    HTTP_FORMAT_XML,
    HTTP_FORMAT_JSON
  };

} // end of namespace http
} // end of namespace algol

#endif
