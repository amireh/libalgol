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

#include "algol/algolSH/connection.hpp"

namespace algol {
namespace admin {
namespace shell {

  connection::connection(boost::asio::io_service& io_service, std::string server, std::string port)
  : admin::connection(io_service),
    resolver_(io_service),
    host_(server),
    port_(port),
    connected_(false)
  {
  }

  connection::~connection() {
  }

  dispatcher& connection::get_dispatcher() {
    return dispatcher_;
  }

  void connection::stop() {
    boost::system::error_code ignored_ec;

    // initiate graceful connection closure & stop all asynchronous ops
    socket_.cancel();
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
    //~ socket_.close(ignored_ec);
  }

  bool connection::connect() {
    if (connected_) {
      return true;
    }

    tcp::resolver::query query(host_, port_);

    // Start a synchronous resolve
    boost::system::error_code ec;
    tcp::resolver::iterator endpoint = resolver_.resolve(query);
    socket_.connect(*endpoint, ec);

    if (ec) {
      // std::cerr << "couldn't connect to server " << host_ << ":" << port_ << ", EC=" << ec << ", aborting\n";
      return false;
    }

    connected_ = true;

    // set KEEPALIVE to true
    socket_.set_option(boost::asio::socket_base::keep_alive(true));

    return connected_;
  }

  bool connection::is_connected() {
    return connected_;
  }
  void connection::disconnect() {
  }

}
}
}
