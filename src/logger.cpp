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

#include <algol/logger.hpp>

namespace algol {

  logger::logger(const char* context)
  : log_(nullptr)
  {
    if (context)
      __create_logger(context);
  }

  logger::logger(string_t context)
  : log_(nullptr)
  {
    if (!context.empty())
      __create_logger(context.c_str());
  }

  logger::logger(const logger& src)
  {
    log_ = src.log_;
  }

  logger::~logger()
  {
    if (log_)
      delete log_;

    log_ = nullptr;
  }

  logger& logger::operator=(const logger& rhs)
  {
    if (this != &rhs) log_ = rhs.log_;

    return *this;
  }

  void logger::assign_uuid(uuid_t uuid) {
    // __destroy_logger();
    // __create_logger( ("{" + uuid + "} " + context_ ).c_str() );
    static_cast<log4cpp::FixedContextCategory*>(log_)->
      setContext( ("{" + uuid + "} " + context_ ).c_str() );
  }

  void logger::assign_uuid(identifiable& obj) {
    assign_uuid(obj.get_uuid());
  }

  log4cpp::Category* logger::__log() {
    return log_;
  }

  void logger::__create_logger(const char* context)
  {
    context_ = string_t(context);

    log_manager &mgr = log_manager::singleton();
    log_ = new log4cpp::FixedContextCategory(mgr.category(), context);
  }

  void logger::__destroy_logger() {
    if (log_)
      delete log_;

    log_ = nullptr;
  }

}
