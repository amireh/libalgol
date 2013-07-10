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

%module(directors="1") algol_py
%{
  #define SWIG_FILE_WITH_INIT
  #include <algol/algol_config.hpp>
%}

%include "cstring.i"
%include "std_string.i"
%include "std_list.i"
%include "std_vector.i"
%include "std_map.i"

typedef std::string string_t;

%include "algol.i"
%include "log4cpp/Category.i"
%include "logger.i"


#ifdef ALGOL_MESSAGING
  %include "messaging/types.i"
  %include "messaging/station.i"
  %include "messaging/channel.i"
  %include "messaging/communicator.i"
  %include "messaging/message.i"
#endif


