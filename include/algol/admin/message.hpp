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

#ifndef H_ALGOL_ADMIN_MESSAGE_H
#define H_ALGOL_ADMIN_MESSAGE_H

#include <vector>
#include <exception>
#include <map>
#include <iostream>
#include <boost/crc.hpp>
#include <boost/asio.hpp>

#include "algol/algol.hpp"

namespace algol {
namespace admin {

  typedef string_t message_uid;

  /*enum class messageType : unsigned char {
    unassigned = 0,
    Request,
    Response
  };*/

  /*enum class message_uid : unsigned char {
    unassigned = 0,
    quit,
    sanity_check
  };*/

  enum class message_feedback : unsigned char {
    unassigned = 0,

    ok,
    error,
    invalid_request,
    invalid_credentials,

    sanity_check
  };

  /**
   * \addtogroup Console
   * @{
   * @class message
   *
   * Message object that is used and handled to represent commands sent
   * by the dakapi shell.
   */
  struct message {

    enum {
      // if length is small enough it will be cast to 1 byte thus we can't
      // depend on uint16_t being interpreted as 2 bytes long.. so we -1
      header_length = 7, // "uidoptionsfeedbacklength"
      footer_length = 4 // "\r\n\r\n"
    };

    // event options
    enum {
      no_format    = 0x01 // events with no format will not be parsed per-property
    };

		typedef	std::map< string_t, string_t > property_t;

    message();
    message(const message_uid inuid, const message_feedback = message_feedback::unassigned, unsigned char options=0);
    message(const message& src);
    message& operator=(const message& rhs);

		~message();

    bool from_stream(boost::asio::streambuf& in);
    void to_stream(boost::asio::streambuf& out) const;

    /** resets event state */
    void reset();

		string_t const& property(string_t inName) const;

		void set_property(const string_t inName, const string_t inValue);
    void set_property(const string_t inName, int inValue);
    bool has_property(const string_t inName) const;
    bool has_option(unsigned char message_option) const;

    friend std::ostream& operator<<(std::ostream&, const message&);
    std::string const& operator[](std::string const& property) const;

    /// debug
		void dump(std::ostream& inStream = std::cout) const;

		message_uid		        uid;
    uint32_t              uid_length;
    unsigned char         options;
    message_feedback      feedback;
    uint32_t              length;
		int                   checksum;
    property_t		        properties;
    uint32_t              rawsz;
    static const char     *footer;
    static const uint32_t max_length; // no single message can be longer than this (2^32-1)
    void                  *any;

    static const string_t null_property_;

    static int _CRC32(const string_t& my_string);
    static string_t _uid_to_string(message_uid);
		void _clone(const message& src);
	};
  /** @} */

} // namespace admin
} // namespace algol

#endif // H_ALGOL_ADMIN_MESSAGE_H
