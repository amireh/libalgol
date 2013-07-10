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

#ifndef H_ALGOL_LOG4CPP_FILE_LOG_LAYOUT_H
#define H_ALGOL_LOG4CPP_FILE_LOG_LAYOUT_H

#include <log4cpp/Portability.hh>
#include <log4cpp/Layout.hh>
#include <memory>

using namespace log4cpp;
namespace algol {

  /** A log4cpp layout that writes to a file. */
  class LOG4CPP_EXPORT algol_file_log_layout : public Layout {
    public:
    algol_file_log_layout();
    virtual ~algol_file_log_layout();

    /**
     * Formats the LoggingEvent in PixyLogLayout style:<br>
     * "timeStamp priority category ndc: message"
     **/
    virtual std::string format(const LoggingEvent& event);
  };
}

#endif // END OF H_ALGOL_LOG4CPP_LOG_LAYOUT_H
