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
%{
  #include <algol/http/request.hpp>
%}

namespace algol {
namespace http {

  class request {
  public:
    request();
    virtual ~request();

    typedef std::vector<header> headers_t;

    string_t  method;
    string_t  uri;
    string_t  body;
    int       http_version_major;
    int       http_version_minor;
    headers_t headers;
    bool      has_content_length;
    size_t    content_length;
    char      format;             /** Found in the HTTP Content-Type header */
    string_t  format_str;         /** The literal representation of format */
    string_t  tolang;
    string_t  apikey;
    uuid_t  uuid;

    enum status_t {
      unassigned,
      valid,
      invalid_method, /** the request method is not a POST */
      invalid_format, /** Content-Type contains an unsupported format */
      invalid_length, /** when Content-Length = 0 */
      missing_apikey, /** there's no Dakwak-APIKey header */
      missing_tolang, /** there's no Dakwak-ToLang header */
      missing_length  /** there's no Content-Length header */
    } status;

    header const& get_header(string_t const& key);

    void reset();
    bool validate();

    string_t pretty_print() const;
    // friend std::ostream& operator<<(std::ostream& s, const request& req);

    template <typename InputIterator>
    inline void
    append_body(InputIterator begin, InputIterator end)
    {
      while (begin != end)
        body.push_back(*begin++);
    }

    /** is this a POST request? */
    bool validate_method();
    bool validate_length();

    /**
     * Determines the format of the request (and the response, in turn) based
     * on the Content-Type HTTP header.
     *
     * Currently supported request/response formats:
     *  1. XML
     */
    bool find_and_validate_format();
    bool find_and_validate_apikey();
    bool find_and_validate_tolang();
  };

} // namespace http
} // namespace algol
