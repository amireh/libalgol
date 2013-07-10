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

#ifndef H_ALGOL_EXCEPTION_H
#define H_ALGOL_EXCEPTION_H

#include <exception>
#include <string>
#include <stdexcept>

namespace algol {

  /* bad conversion
   *
   * thrown when an argument passed to utility::convertTo<> is not a number
   * and thus can not be converted
   **/
  class bad_conversion : public std::runtime_error {
  public:
    inline bad_conversion(const std::string& s)
    : std::runtime_error(s)
    { }
  };

  /** thrown when an identifiable object's UUID is being re-assigned */
  class integrity_violation : public std::runtime_error {
	public:
		inline integrity_violation(const std::string& s)
		: std::runtime_error(s)
		{ }
	};

  /* thrown when an uncompiled algol::regex object is used to match a string */
	class invalid_regex : public std::runtime_error {
	public:
		inline invalid_regex(const std::string& s)
		: std::runtime_error(s)
		{ }
	};

  /* thrown when a algol::regex object fails to compile a certain pattern */
	class invalid_regex_pattern : public std::runtime_error {
	public:
		inline invalid_regex_pattern(const std::string& s)
		: std::runtime_error(s)
		{ }
	};

  /* thrown when an uncompiled algol::regex object is asked to re-compile */
  class illegal_operation : public std::runtime_error {
  public:
    inline illegal_operation(const std::string& s)
    : std::runtime_error(s)
    { }
  };

  /* thrown when a messaging channel cannot be opened (connection to messaging daemon can't be established) */
	class connection_error : public std::runtime_error {
	public:
		inline connection_error(const std::string& s)
		: std::runtime_error(s)
		{ }
	};

  namespace analytics {

    /**
     * @class out_of_ink
     *
     * An analytics::tracker instance is asked to attach a sheet when
     * a sheet has already been attached.
     */
    class out_of_ink : public std::runtime_error {
    public:
      inline out_of_ink()
      : std::runtime_error("Illegal attempt to build or attach an analytics sheet; a sheet is already attached.")
      { }
    };
  }

  namespace lua {
    class script_error : public std::runtime_error {
    public:
      inline script_error(const std::string& s)
      : std::runtime_error(s)
      { }
    };

  }
} // end of namespace Pixy

#endif
