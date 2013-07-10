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

#ifndef H_ALGOL_ADMIN_BOT_H
#define H_ALGOL_ADMIN_BOT_H

#include "algol/algol.hpp"
#include "algol/logger.hpp"
#include "algol/monitor.hpp"
#include "algol/admin/connection.hpp"

namespace algol {
namespace admin {

  class console;

  /**
   * \addtogroup Console
   * @{
   * @class bot
   * @brief
   * Bots are used to receive and respond to commands and messages sent through algolSH and the console.
   *
   * You should subclass this and make your own bot with custom handlers to the commands you want to
   * support. By default, the bot provides a "help" command that will list all the registered & supported
   * commands (just like running a CLI application with --help would).
   *
   * An example binding the command "svc-status" to the method on_svc_status():
   *
   * @code
      class my_bot : public bot {
      public:

        inline my_bot(connection* c) : bot(c) {
          bind("svc-status", "foobar", boost::bind(&my_bot::on_svc_status, this, _1));
        }

        inline virtual ~my_bot() {
        }

        inline void on_svc_status(const message& in) {
          // do logic here
          // ...
          // prepare response
          message out("svc-status");
          out["Requested Service"] = "available"
          send(out);
        }
      };
  *
  * To learn more about messages, check out algol::admin::message
  */

  class bot : public logger
  {
  public:

    bot(connection*);
    bot(const bot&) = delete;
    bot& operator=(const bot&) = delete;
    virtual ~bot();

    /**
     * Will be called when the connection starts, this is where you should bind
     * the supported commands.
     */
    virtual void start();

    /**
     * Registers the command identified by @command, with a useful description
     * and binds the given handler to it so that whenever a message with the same identifier
     * is received, the handler will be called back.
     *
     * The command will automatically be available to the "help" list of commands when you bind
     * using this method.
     */
    void bind(message_uid const& command, string_t const& command_description, std::function<void(const message&)>);

    /**
     * Sends the given message back to the shell client.
     *
     * This is merely a helper for admin::connection::send(const message&)
     *
     * @note
     *  The message will be cloned.
     */
    void send(const message&);

    /**
     * Sends a confirmation reply that the command was accepted and carried out
     * with @status as a small message to convey the status.
     *
     * @note
     *  The message will be cloned.
     */
    void confirm(const message& msg, string_t const& status);

    /**
     * Sends a rejected command reply to the shell with @error as a description
     * of why the command was rejected.
     *
     * @param msg
     *  The original command message you want to reject.
     *
     * @note
     *  The message will be cloned.
     */
    void reject(const message& msg, string_t const& error, message_feedback = message_feedback::error);

  protected:
    typedef std::map<string_t, string_t> commands_t;
    commands_t commands_;

  protected:
    friend class console;

    virtual void on_help(const message&);
    virtual void on_quit(const message&);

    /** used by the console when shutting down the connection & its bot */
    connection* conn();

  private:
    connection* conn_;
  };

  /** @} */
}
}

#endif
