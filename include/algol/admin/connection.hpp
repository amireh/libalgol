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

#ifndef H_ALGOL_ADMIN_CONNECTION_H
#define H_ALGOL_ADMIN_CONNECTION_H

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "algol/logger.hpp"
#include "algol/monitor.hpp"
#include "algol/admin/dispatcher.hpp"
#include "algol/admin/message.hpp"

namespace algol {
namespace admin {

  class connection;
  //typedef boost::shared_ptr<connection> connection_ptr;
  typedef boost::function<void(connection*)> stop_cb_t;

  class bot;

  /**
   * \addtogroup Console
   * @{
   * @class connection
   * A privileged admin connection from a system administrator used
   * by the Console.
   **/
  class connection
  /*: public boost::enable_shared_from_this<connection>,*/
    : public logger
  {
  public:
    explicit connection(boost::asio::io_service& io_service);
    virtual ~connection();

    boost::asio::ip::tcp::socket& socket();

    virtual void start();
    virtual void stop();

    /**
     * Send a message to the client.
     *
     * The message's feedback field will be set to message_feedback::ok if it's unassigned.
     */
    virtual void send(message&);

    /**
     * The dispatcher responsible for notifying listeners to incoming messages.
     *
     * This handle is necessary for you to
     */
    dispatcher& get_dispatcher();

    /** Register a callback that will be called when this connection has stopped. */
    void assign_stop_cb(stop_cb_t);

  protected:
    boost::asio::ip::tcp::socket socket_;
    boost::asio::strand strand_;
    boost::asio::mutable_buffer body_;
    boost::asio::streambuf request_;
    boost::asio::streambuf response_;

    dispatcher dispatcher_;
    message outbound_, inbound_;

    stop_cb_t stop_cb_;

  protected:
    friend class console;

    virtual void read();
    virtual void on_read( const boost::system::error_code& error, std::size_t bytes_transferred);

    virtual void do_send(message&);

    /**
     * The bot that is representing this connection.
     *
     * Used by the console when shutting down the connection & its bot.
     */
    bot* get_bot();

  private:
    friend class bot;

    bot          *bot_; /** the bot assigns itself to us when it's constructed */
  };

  /** @} */

} // namespace admin
} // namespace algol

#endif
