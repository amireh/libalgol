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

#ifndef H_ALGOL_ADMIN_CONSOLE_H
#define H_ALGOL_ADMIN_CONSOLE_H

#include "algol/algol.hpp"
#include "algol/logger.hpp"
#include "algol/configurable.hpp"
#include "algol/admin/bot.hpp"
#include "algol/admin/connection.hpp"

#include <list>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <functional>

namespace algol {
namespace admin {

  /**
   * \addtogroup Console
   * @{
   * @class console
   *
   * The console creates a algolSH compatible interface for the user application to
   * provide a controllable shell interface.
   *
   * Using the console will allow your application to receive and send messages
   * via the algol shell, algolSH, with system administrators or scripts, cron jobs, etc.
   * To actually register the services that can be provided by the shell interface,
   * refer to algol::admin::bot.
   *
   * The console can be configured using the algol::configurable interface and
   * subscribes to the 'console' configuration context. For a list of supported options
   * see console::config_t
   */
  class console : public logger, public configurable {
  public:

    /**
     * Creates the console interface that will accept shell connections. The console
     * will not start accepting immediately until console::launch() is called.
     *
     * Although this is not a singleton class, there is rarely the need to maintain
     * more than one instance of the console throughout the entire application's life.
     *
     * @warn
     * Creating more than one console instances is not tested nor supported.
     */
    console(boost::asio::io_service&);
    virtual ~console();

    /**
     * Register a custom factory method that returns a newly created bot that will be
     * used to manage the shell connections. See algol::admin::bot for more info.
     *
     * @warn
     * Custom bot implementations must abide the Rule of Three:
     *  (operator=, copy ctor, and virtual dtor)
     *  more info: (http://en.wikipedia.org/wiki/Rule_of_three_(C%2B%2B_programming))
     */
    typedef std::function<bot* (connection*)> bot_maker_t;
    void register_bot_maker(bot_maker_t);

    /**
     * Begins accepting incoming shell connections.
     *
     * @remark
     * The acceptor runs in a seperate boost::thread which will be terminated
     * when console::shutdown() is called.
     */
    void launch();

    /**
     * Abruptly terminates all current connections and stops accepting new ones.
     */
    void shutdown();

    /** Overridden from algol::configurable */
    virtual void set_option(const string_t&, const string_t&);

  private:
    boost::asio::io_service         &io_service_;
    boost::asio::ip::tcp::acceptor  acceptor_;
    boost::asio::strand             strand_;

    bot             *new_bot_;
    connection      *new_conn_;
    bot_maker_t     bot_maker_;
    std::list<bot*> bots_;
    uint64_t        nr_bots_;
    bool            launched_;

    struct config_t {
      string_t  interface;  /* default: 127.0.0.1 */
      string_t  port;       /* default: 60300 */
    } config_;

  private:
    void accept();

  protected:
    friend class connection;
    friend class bot;

    void handle_accept(const boost::system::error_code &e);
    void close(connection*);
  };

  /** @} */
}
}

#endif
