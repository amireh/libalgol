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

%module lua_algol

#ifndef H_ALGOL_HTTP_HEADER_H
#define H_ALGOL_HTTP_HEADER_H

%{
  #include <algol/http/header.hpp>
%}

namespace algol {
namespace http {

  struct header
  {
    header()
    : is_algol_header(false) {

    }

    header(string_t const& k, string_t const& v, bool f = false)
    : name(k),
      value(v),
      is_algol_header(f)
    {

    }
    string_t name;
    string_t value;

    /** algol headers start with X-algol-[HEADER] */
    bool is_algol_header;
  };

} // end of namepsace http
} // end of namespace algol

#endif // H_ALGOL_REQUEST_H
