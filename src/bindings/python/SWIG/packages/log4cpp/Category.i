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
  #include <log4cpp/Category.hh>
  #include <log4cpp/Portability.hh>
  #include <log4cpp/Appender.hh>
  #include <log4cpp/LoggingEvent.hh>
  #include <log4cpp/Priority.hh>
  #include <log4cpp/CategoryStream.hh>
  #include <log4cpp/threading/Threading.hh>
  #include <log4cpp/convenience.h>

  #include <map>
  #include <vector>
  #include <cstdarg>
  #include <stdexcept>
%}

namespace log4cpp {

  /**
   * This is the central class in the log4j package. One of the distintive
   * features of log4j (and hence log4cpp) are hierarchal categories and
   * their evaluation.
   **/
  class Category {
  private:
    Category();

  public:
    virtual ~Category();
    /**
     * Log a message with debug priority.
     * @param stringFormat Format specifier for the string to write
     * in the log file.
     * @param ... The arguments for stringFormat
     **/
    // void debug(const char* stringFormat, ...) throw();

    /**
     * Log a message with debug priority.
     * @param message string to write in the log file
     **/
    void debug(const std::string& message) throw();

    /**
     * Log a message with info priority.
     * @param stringFormat Format specifier for the string to write
     * in the log file.
     * @param ... The arguments for stringFormat
     **/
    // void info(const char* stringFormat, ...) throw();

    /**
     * Log a message with info priority.
     * @param message string to write in the log file
     **/
    void info(const std::string& message);

    /**
     * Log a message with notice priority.
     * @param stringFormat Format specifier for the string to write
     * in the log file.
     * @param ... The arguments for stringFormat
     **/
    void notice(const char* stringFormat, ...) throw();

    /**
     * Log a message with notice priority.
     * @param message string to write in the log file
     **/
    void notice(const std::string& message) throw();

    /**
     * Log a message with warn priority.
     * @param stringFormat Format specifier for the string to write
     * in the log file.
     * @param ... The arguments for stringFormat
     **/
    void warn(const char* stringFormat, ...) throw();

    /**
     * Log a message with warn priority.
     * @param message string to write in the log file
     **/
    void warn(const std::string& message) throw();

    /**
     * Log a message with error priority.
     * @param stringFormat Format specifier for the string to write
     * in the log file.
     * @param ... The arguments for stringFormat
     **/
    void error(const char* stringFormat, ...) throw();

    /**
     * Log a message with error priority.
     * @param message string to write in the log file
     **/
    void error(const std::string& message) throw();

    /**
     * Log a message with crit priority.
     * @param stringFormat Format specifier for the string to write
     * in the log file.
     * @param ... The arguments for stringFormat
     **/
    void crit(const char* stringFormat, ...) throw();

    /**
     * Log a message with crit priority.
     * @param message string to write in the log file
     **/
    void crit(const std::string& message) throw();

    /**
     * Log a message with alert priority.
     * @param stringFormat Format specifier for the string to write
     * in the log file.
     * @param ... The arguments for stringFormat
     **/
    void alert(const char* stringFormat, ...) throw();

    /**
     * Log a message with alert priority.
     * @param message string to write in the log file
     **/
    void alert(const std::string& message) throw();

    /**
     * Log a message with emerg priority.
     * @param stringFormat Format specifier for the string to write
     * in the log file.
     * @param ... The arguments for stringFormat
     **/
    void emerg(const char* stringFormat, ...) throw();

    /**
     * Log a message with emerg priority.
     * @param message string to write in the log file
     **/
    void emerg(const std::string& message) throw();

    /**
     * Log a message with fatal priority.
     * NB. priority 'fatal' is equivalent to 'emerg'.
     * @since 0.2.7
     * @param stringFormat Format specifier for the string to write
     * in the log file.
     * @param ... The arguments for stringFormat
     **/
    void fatal(const char* stringFormat, ...) throw();

    /**
     * Log a message with fatal priority.
     * NB. priority 'fatal' is equivalent to 'emerg'.
     * @since 0.2.7
     * @param message string to write in the log file
     **/
    void fatal(const std::string& message) throw();
  };

}
