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

#ifndef H_ALGOL_SHELL_H
#define H_ALGOL_SHELL_H

#include "algol/algol.hpp"
#include "algol/plugin_manager.hpp"
#include "algol/algolSH/connection.hpp"
#include "algol/algolSH/delegate.hpp"

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <list>
#include <vector>
#include <boost/thread.hpp>
#include <map>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

using boost::asio::ip::tcp;

namespace algol {
namespace admin {
namespace shell {

  /**
   * \addtogroup Console
   * @{
   * @class algolSH
   *
   * algolSH is the CUI to the shell, aka Console, libalgol provides to manage
   * target applications.
   *
   * algolSH provides support for target applications via plug-ins. For more info
   * see https://algol.atlassian.net/wiki/display/be/algolSH.
   */
  typedef std::map<string_t, delegate*> delegates_t;

  class algolSH {
  public:

    static algolSH& singleton();

    typedef struct {
      bool      print_status; /** suffixes response with [ OK ] or [ FAILED ] based on response feedback (default: true) */
      bool      interactive;  /** in interactive mode the input feed is terminated manually by the user (default: true) */
      string_t  target;       /** the target client application to connect to (this only affects the loaded delegate plugin - if any) */
      string_t  host;
      string_t  port;
      string_t  carat;        /** the carat displayed before each feed line (default: %> ) */
    } config_t;

    algolSH(boost::asio::io_service&, config_t &);
    virtual ~algolSH();

    /**
     * Begins the feed process.
     *
     * If @command is not empty and algolSH is not running in interactive mode,
     * the command will be sent to the client application and algolSH will terminate.
     */
    void run(string_t command = "");

    /**
     * Loads all delegate plugins from the algolSH configuration file.
     *
     * On Linux, algolSH plugin listing is in /usr/local/share/algolSH/plugins
     */
    void load_delegates();

    /**
     * Binds the given delegate to its target. If the target specified at run-time
     * is the same as this delegate's, the delegate will be used to handle the connection.
     *
     * When no delegate is attached, the "generic" delegate is used.
     */
    void register_delegate(delegate*);

    /** All loaded & registered delegates */
    delegates_t const& delegates() const;

    /** Injects a SIGINT causing algolSH to stop. */
    void quit();

    /** Sends a message to the client application. This is mostly used by delegates. */
    void send(message&);

    config_t const& config() const;

  public:
    // Helpers

    /** Returns a set of the default options, used by main() when parsing CLI arguments */
    static config_t defaults();

    enum color_id {
      RED,
      GREEN
    };

    /** Returns a colored version of the given text, see algolSH::color_id for available colors */
    static string_t colored_text(string_t const&, color_id);

    /** Running in interactive mode? (default: true) */
    bool is_interactive() const;

  private:
    static algolSH* __instance;

    boost::asio::io_service &io_service_;
    boost::asio::deadline_timer timer_;
    boost::thread_group workers_;
    config_t config_;

    connection_ptr conn_;
    bool running_;

  private:
    delegates_t delegates_;
    delegate    *delegate_;

  private:
    /** Prints the carat and flushes STDOUT */
    void refresh_shell(const message&);

    /** Quits after a command has been sent (this is used in non-interactive mode) */
    void eval_and_quit(const message&);
  };

  /** @} */
}
}
}

#endif
