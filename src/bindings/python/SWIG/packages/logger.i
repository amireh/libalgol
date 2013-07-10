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

%module algol_py
%{
  #include "algol/logger.hpp"
%}

namespace algol {

  /**
   * @class logger
   * Logger instances can log messages using the algol::log_manager system.
   */
  class logger
  {
  public:

    /**
     * @param context
     *  A prefix prepended to every message logged by this system. This should
     *  normally denote the name of the logging module.
     */
    logger(string_t);
    virtual ~logger();

    log4cpp::Category* __log();
    protected:
    log4cpp::Category* log_;

    void __create_logger(const char* context);
  }; // end of logger class

} // end of namespace algol
