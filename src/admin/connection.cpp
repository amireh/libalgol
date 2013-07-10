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

#include "algol/admin/connection.hpp"

namespace algol {
namespace admin {

  connection::connection(boost::asio::io_service& io_service)
  : logger("admin_connection"),
    socket_(io_service),
    strand_(io_service),
    request_(message::max_length),
    response_(message::max_length),
    dispatcher_(io_service),
    stop_cb_(NULL),
    bot_(0)
  {
  }

  connection::~connection() {
    bot_ = 0;
  }

  boost::asio::ip::tcp::socket& connection::socket() {
    return socket_;
  }

  dispatcher& connection::get_dispatcher() {
    return dispatcher_;
  }

  bot* connection::get_bot() {
    return bot_;
  }

  void connection::assign_stop_cb(stop_cb_t in_cb)
  {
    stop_cb_ = in_cb;
  }

  void connection::start() {
    socket_.set_option(boost::asio::ip::tcp::no_delay(true));
    boost::asio::socket_base::non_blocking_io command(true);
    socket_.io_control(command);

    read();
  }

  void connection::stop() {
    boost::system::error_code ignored_ec;

    // initiate graceful connection closure & stop all asynchronous ops
    socket_.cancel();
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
    //~ socket_.close(ignored_ec);
    if (stop_cb_)
      stop_cb_(this);

    //~ strand_.post( boost::bind(&server::_admin_close, &server::singleton(), shared_from_this()));
    //~ message quit(message_uid::quit);
    //~ dispatcher_.deliver(quit, true);
  }

  void connection::read() {
    inbound_.reset();
    request_.consume(request_.size());

    async_read_until(
      socket_,
      request_,
      message::footer,
      boost::bind(
        &connection::on_read,
        this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred
      )
    );
  }

  void
  connection::on_read( const boost::system::error_code& e, std::size_t)
  {
    if (!e) {
      // we use a loop since a buffer might hold more than 1 msg
      while (request_.size() > 0 && inbound_.from_stream(request_)) {
        dispatcher_.deliver(inbound_);
      }

      // read next message
      read();

    } else {
      stop();
    }
  }

  void connection::send(message &msg) {
    strand_.post(boost::bind(&connection::do_send, this, msg));
  }

  void connection::do_send(message& msg)
  {
    outbound_ = message(msg);

    if (outbound_.feedback == message_feedback::unassigned)
      outbound_.feedback = message_feedback::ok;

    outbound_.to_stream(response_);

    boost::system::error_code ec;
    size_t n = boost::asio::write(socket_, response_.data(), boost::asio::transfer_all(), ec);

    if (!ec) {
      response_.consume(n);
    } else
      stop();
  }

} // namespace admin
} // namespace algol
