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
  #include "algol/algol.hpp"
%}

namespace algol
{
  typedef unsigned short  ushort_t;
  typedef unsigned int    uint_t;
  typedef std::string     string_t;

  void algol_init(string_t app_name,
                   string_t app_version_major,
                   string_t app_version_minor,
                   string_t app_version_patch,
                   string_t app_version_build);
  void algol_cleanup();

  /**
   * @struct algol_app
   * Globally accessible information about the running application.
   *
   * @note
   * Some fields require algol_init() to be populated.
   */
  typedef struct {
    string_t name;          // Populated by ALGOL_APP_NAME
    string_t hostname;      // Populated by the FQDN of the host machine on algol_init()
    string_t version;       // Populated by ALGOL_APP_VERSION
    string_t version_major; // Populated by ALGOL_APP_VERSION_MAJOR
    string_t version_minor; // Populated by ALGOL_APP_VERSION_MINOR
    string_t version_patch; // Populated by ALGOL_APP_VERSION_PATCH
    string_t version_build; // Populated by ALGOL_APP_VERSION_BUILD

    // The fully-qualified name that is unique across the network, format:
    // name-version@hostname
    string_t fqn;
  } algol_app_t;

  algol_app_t const& algol_app();
}
