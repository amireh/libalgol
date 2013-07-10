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

#include "algol/admin/console.hpp"
#include "algol/utility.hpp"

namespace algol {
namespace admin {

  console::console(boost::asio::io_service& io_service)
  : logger("console"),
    configurable({ "console" }),
    io_service_(io_service),
    acceptor_(io_service_),
    strand_(io_service_),
    new_bot_(nullptr),
    new_conn_(nullptr),
    launched_(false)
  {
    config_.interface = "127.0.0.1";
    config_.port = "60400";

    bot_maker_ = [](connection* c) -> bot* {
      return new bot(c);
    };
  }

  console::~console() {
    if (launched_)
      shutdown();

    bots_.clear();

    if (new_bot_)
      delete new_bot_;
    new_bot_ = nullptr;

    if (new_conn_)
      delete new_conn_;
    new_conn_ = nullptr;
  }

  void console::launch()
  {
    log_->infoStream() << "launching @ " << config_.interface << ":" << config_.port;
    // admin connections interface
    // open the client acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
    {
      boost::asio::ip::tcp::resolver resolver(io_service_);
      boost::asio::ip::tcp::resolver::query query(config_.interface, config_.port);
      boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
      acceptor_.open(endpoint.protocol());
      acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
      acceptor_.bind(endpoint);
      acceptor_.listen();
    }

    this->accept();

    launched_ = true;
    log_->infoStream() << "launched & ready to service";
  }

  void console::shutdown()
  {
    acceptor_.close();
    delete new_conn_;
    new_conn_ = nullptr;

    launched_ = false;
    log_->infoStream() << "shut down";
  }

  void console::accept() {
    log_->infoStream() << "accepting a connection";

    new_conn_ = new connection(io_service_);
    new_conn_->assign_stop_cb(boost::bind(&console::close, this, _1));
    new_bot_ = bot_maker_(new_conn_);

    // accept connections_
    acceptor_.async_accept(
      new_conn_->socket(),
      boost::bind(&console::handle_accept, this, boost::asio::placeholders::error));
  }

  void console::handle_accept(const boost::system::error_code &e) {
    if (!e)
    {
      // start the bot connection
      new_conn_->start();
      new_bot_->start();
      bots_.push_back(new_bot_);
      ++nr_bots_;

      this->accept();
    } else {
      log_->errorStream() << "couldn't accept connection! " << e;
      throw std::runtime_error("unable to accept connection, see log for more info");
    }
  }

  void console::close(admin::connection* in_conn) {
    // TODO: do we really need to strand-ify this?
    strand_.post( [&, in_conn]() {
      bot* lbot = in_conn->get_bot();
      bots_.remove(lbot);
      delete lbot->conn();
      delete lbot;
    });
  }

  void console::set_option(const string_t& k, const string_t& v) {
    if (k == "port")
      config_.port = v;
    else if (k == "interface")
      config_.interface = v;
  }

  void console::register_bot_maker(bot_maker_t maker) {
    bot_maker_ = maker;
  }
}
}
