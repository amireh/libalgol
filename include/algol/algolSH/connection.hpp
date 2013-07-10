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

#ifndef H_ALGOL_ADMIN_SHELL_CONNECTION_H
#define H_ALGOL_ADMIN_SHELL_CONNECTION_H

#include "algol/admin/connection.hpp"

namespace algol {
namespace admin {
namespace shell {

  /**
   * \addtogroup Console
   * @{
   * @class connection
   * The underlying connection between the console and the target application.
   */
  class connection : public boost::enable_shared_from_this<connection>, public admin::connection
  {
    public:
    /// Construct a connection with the given io_service.
    explicit connection(boost::asio::io_service&, std::string host, std::string port);
    virtual ~connection();

    bool connect();
    bool is_connected();
    void disconnect();

    /*virtual boost::shared_ptr<connection> shared_from_this() {
      return boost::static_pointer_cast<connection>(admin::connection::shared_from_this());
    }*/

    virtual void stop();

    dispatcher& get_dispatcher();

    protected:

    tcp::resolver resolver_;
    string_t      host_, port_;
    bool          connected_;

    void handle_resolve(const boost::system::error_code& err,
        tcp::resolver::iterator endpoint_iterator);
    void handle_connect(const boost::system::error_code& err,
        tcp::resolver::iterator endpoint_iterator);
  };

  typedef boost::shared_ptr<connection> connection_ptr;

  /** @} */
}
}
}

#endif // H_ALGOL_ADMIN_SHELL_CONNECTION_H
