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

#include "algol/log4cpp/syslog_log_layout.hpp"
#include <log4cpp/Priority.hh>
#ifdef LOG4CPP_HAVE_SSTREAM
#include <sstream>
#endif
#include <iostream>

namespace algol {

  algol_syslog_log_layout::algol_syslog_log_layout() {
  }

  algol_syslog_log_layout::~algol_syslog_log_layout() {
  }

  std::string algol_syslog_log_layout::format(const LoggingEvent& event) {

		std::ostringstream message;

    const std::string& priorityName = Priority::getPriorityName(event.priority);

		// start off with priority
		message << "[" << priorityName[0]	<< "] ";

		// append NDC
		if (event.ndc != "")
			message << event.ndc << ": ";

		message << event.message << "\n";

    return message.str();
  }

}
