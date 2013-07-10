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

#ifndef H_ALGOL_LOG_MANAGER_H
#define H_ALGOL_LOG_MANAGER_H

// dakapi
#include "algol/algol.hpp"
#include "algol/log.hpp"
#include "algol/configurable.hpp"

#include <boost/filesystem.hpp>

namespace algol {

  /**
   * \addtogroup Core
   * @{
   * @class log_manager
   *
   * Manages a logging system through log4cpp that can be used by an application
   * to log messages to a number of output devices; stdout, a file, or syslog.
   *
   * Logging devices and the format of messages are configurable.
   *
   * Actual logging of the messages should be done by by instances derived from
   * algol::logger (see algol/algol_logger.hpp).
   */
  class log_manager : public configurable {
  public:

    /* the log manager's config context is "log manager" */
    typedef struct {
      string_t log_device;   /** possible values: 'stdout' or 'syslog' or 'file', default: 'file' */
      string_t log_level;    /** possible values: 'debug', 'notice', 'info', 'warn', 'error', default: 'debug' */

      /* the following apply only when logging to a file */
      string_t log_dir;      /** default: "log", the log file will be in /path/to/app/log/log_name.log */
      string_t log_name;     /** default: "algol.log" */
      string_t log_filesize; /** value format: "[NUMBER][B|K|M]", default: 10M */

      string_t app_name;
      string_t app_version;
      string_t app_website;

      bool     silent;
    } config_t;

    config_t config_;

    static log_manager& singleton();

    typedef log4cpp::Category log_t;

    /** The unique log4cpp::category identifier, used internally by logger instances */
    string_t const& category();

    virtual ~log_manager();
    log_manager(const log_manager&) = delete;
    log_manager& operator=(const log_manager&) = delete;

    /**
     * Initializes the logging system, this must be called before the log_manager
     * is configured.
     *
     * @param category_name
     *  A unique identifier for the primary logging category used to create
     *  instances of loggers. (Note: this string will not be visible in logs.)
     **/
    void init(string_t category_name = "algol");

    /**
     * If you init() the log manager, you must call this when you're done
     * to free allocated resources.
     *
     * @warning
     * Any attempt to log messages once the log manager is cleaned up might
     * result in undefined behaviour.
     */
    void cleanup();

    /**
     * Re-builds the logging system with the current configuration settings.
     *
     * @note
     * For internal implementation reasons, the log manager does not subscribe
     * itself to the "log manager" configuration context automatically; you must
     * do that somewhere BEFORE the configuration is parsed.
     *
     * IE, somewhere before you parse configuration file:
     *  configurator::subscribe(&log_manager::singleton(), "log manager");
     */
    virtual void configure();

    /** overridden from algol::configurable */
    virtual void set_option(string_t const& key, string_t const& value);

    /** a log that can be used by any entity that is not a derivative of algol::logger */
    log_t* log();

    static void silence();

  private:
    explicit log_manager();
    static log_manager* __instance;

    log4cpp::Appender *log_appender_;
    log4cpp::Layout   *log_layout_;
    log4cpp::Category *log_category_;
    log4cpp::Category	*log_;
    log4cpp::Category	*anonymous_log_;

    string_t category_name_;
    static bool silent_;
  };

# ifndef ALGOL_LOG
#   define ALGOL_LOG algol::log_manager::singleton().log()
# endif

  /** @} */

} // namespace algol

#endif // H_ALGOL_LOG_MANAGER_H
