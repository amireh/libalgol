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

#ifndef H_ALGOL_HTTP_REPLY_H
#define H_ALGOL_HTTP_REPLY_H

%{
  #include <algol/http/reply.hpp>
%}

namespace algol {
namespace http {

  class request;

  /** a reply to be sent to a client */
  class reply
  {
  public:

    reply();
    virtual ~reply();

    /** the HTTP status of the reply */
    enum status_type
    {
      ok = 200,
      created = 201,
      accepted = 202,
      no_content = 204,
      multiple_choices = 300,
      moved_permanently = 301,
      moved_temporarily = 302,
      not_modified = 304,
      bad_request = 400,
      unauthorized = 401,
      forbidden = 403,
      not_found = 404,
      internal_server_error = 500,
      not_implemented = 501,
      bad_gateway = 502,
      service_unavailable = 503,
      invalid_json_stream = 1000
    } status;

    char format;

    /** the headers to be included in the reply */
    std::vector<header> headers;

    /** the content to be sent in the reply */
    std::string body;

    /**
     * Convert the reply into a vector of buffers. The buffers do not own the
     * underlying memory blocks, therefore the reply object must remain valid and
     * not be changed until the write operation has completed.
     **/
    std::vector<boost::asio::const_buffer> to_buffers() const;

    /** get a stock reply */
    static reply stock_reply(reply::status_type status);

    void from_request(const request&);

    header& get_header(string_t const& key);

    // static const char* footer;
  };

  namespace status_strings {
    std::string const& to_string(reply::status_type status);
  }
}
}

#endif
