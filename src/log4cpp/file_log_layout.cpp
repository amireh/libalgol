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

#include "algol/log4cpp/file_log_layout.hpp"
#include "algol/algol.hpp"
#include "log4cpp/Priority.hh"
#include "log4cpp/FactoryParams.hh"
#ifdef LOG4CPP_HAVE_SSTREAM
#include <sstream>
#endif
#include <iostream>
#include <iomanip>
#include <time.h>

namespace algol {

  algol_file_log_layout::algol_file_log_layout()
  {
  }

  algol_file_log_layout::~algol_file_log_layout() {
  }

  std::string algol_file_log_layout::format(const LoggingEvent& event) {

		std::ostringstream message;

    const std::string& priorityName = Priority::getPriorityName(event.priority);

    // no need to print timestamps if we're using syslog
    struct tm *pTime;
    time_t ctTime; time(&ctTime);
    pTime = localtime( &ctTime );
    message
      << std::setw(2) << std::setfill('0') << pTime->tm_mon
      << '-' << std::setw(2) << std::setfill('0') << pTime->tm_mday
      << '-' << std::setw(4) << std::setfill('0') << pTime->tm_year + 1900
      << ' ' << std::setw(2) << std::setfill('0') << pTime->tm_hour
      << ":" << std::setw(2) << std::setfill('0') << pTime->tm_min
      << ":" << std::setw(2) << std::setfill('0') << pTime->tm_sec
      << " ";


    // start off with priority
    message << "[" << priorityName[0] << "] ";

#ifdef ALGOL_LOG_FQN
    message << algol_app().fqn << " ";
#endif
		// append NDC
		if (event.ndc != "")
			message << event.ndc << ": ";

		message << event.message << "\n";

    return message.str();
  }
}
