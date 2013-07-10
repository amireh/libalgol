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

#include <algol/http/request.hpp>
#include <algol/utility.hpp>

namespace algol {
namespace http {

  request::request() {
    reset();
  }

  request::~request() {

  }

  void request::reset()
  {
    headers.clear();
    has_content_length = false;
    content_length = 0;
    http_version_minor = 0;
    http_version_major = 0;
    method.clear();
    uri.clear();
    body.clear();
    format_str.clear();
    status = status_t::valid;
  }

  bool request::validate_method()
  {
    return true;
  }

  bool request::validate_length()
  {
    if (!has_content_length)
    {
      status = status_t::missing_length;
      return false;
    } else if( content_length == 0 )
    {
      status = status_t::invalid_length;
      return false;
    }

    return true;
  }

  bool request::find_and_validate_format()
  {
    format = HTTP_FORMAT_NONE;

    for (header& h : headers)
    {
      if (utility::ci_find_substr(h.name, "Content-Type") != std::string::npos)
      {
        if (h.value.find( "text/html" ) != string_t::npos) {
          format = HTTP_FORMAT_HTML;
        } else if (h.value.find("application/xml") != string_t::npos) {
          format = HTTP_FORMAT_XML;
        } else if (h.value.find("application/json") != string_t::npos) {
          format = HTTP_FORMAT_JSON;
        }
        format_str = h.value; // used for logging/debugging
        break;
      }
    }

    if (format == HTTP_FORMAT_NONE) {
      status = status_t::invalid_format;
      return false;
    }

    return true;
  }

  bool request::validate()
  {
    return
      validate_method()
      && validate_length()
      && find_and_validate_format();
  }

  std::ostream& operator<<(std::ostream& s, const request& req)
  {
    s << req.method
      << " HTTP" << req.http_version_major << "." << req.http_version_minor
      << " => " << req.uri;
    return s;
  }

  string_t request::pretty_print() const
  {
    std::ostringstream buf;
    buf << *this;
    return buf.str();
  }

  static header NIL_HEADER = header("NIL_HEADER", "NIL_HEADER");

  header const& request::get_header(string_t const& key)
  {
    std::for_each(headers.begin(), headers.end(), [&](header const& h) -> header const& {
      if (h.name == key)
        return h;

      return NIL_HEADER;
    });

    return NIL_HEADER;
  }


} // namespace http
} // namespace algol

