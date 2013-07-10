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

#ifndef H_ALGOL_ADMIN_DISPATCHER_H
#define H_ALGOL_ADMIN_DISPATCHER_H

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <exception>
#include <stdexcept>
#include <map>
#include <deque>
#include "message.hpp"
 #include <functional>

using boost::asio::ip::tcp;
namespace algol {
namespace admin {

  /**
   * \addtogroup Console
   * @{
   * @class dispatcher
   * Dispatches messages received over admin connections to subscribed
   * parties.
   */
  class dispatcher {
    public:
      typedef std::function<void(const message&)> msg_handler_t;

      dispatcher(boost::asio::io_service&);
      virtual ~dispatcher();

      /**
       * Registers the given callback to be notified whenever a message of the
       * specified UID is received and hooked.
       */
      template <typename T>
      void bind(message_uid uid, T* inT, void (T::*handler)(const message&)) {
        bind(uid, boost::bind(handler, inT, _1));
      }

      void bind(message_uid uid, msg_handler_t handler) {
        // register message if it isn't already
        msg_handlers_t::iterator binder = msg_handlers_.find(uid);
        if (binder == msg_handlers_.end())
        {
          binder = msg_handlers_.insert(make_pair(uid, std::vector<msg_handler_t>() )).first;
        }

        binder->second.push_back( handler );
      }

      /** Removes all callbacks registerd to this message. */
      void unbind(message_uid uid);

      /**
       * Delivers a local copy of the message to bound handlers.
       *
       * @note: the message might not be dispatched immediately, as it will be
       * queued internally and later on dispatched through the strand object
       * unless the immediate flag is set
       */
      void deliver(const message&, bool immediate = false);

      /**
       * Clears all message subscription.
       */
      void reset();

    private:
      void dispatch();

      boost::asio::strand strand_;

      typedef std::map< message_uid , std::vector<msg_handler_t> > msg_handlers_t;
      msg_handlers_t msg_handlers_;

      std::deque<message> events;
  };

  /** @} */

} // namespace admin
} // namespace algol

#endif // H_ALGOL_ADMIN_DISPATCHER_H
