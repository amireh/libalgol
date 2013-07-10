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

#include "algol/admin/dispatcher.hpp"

namespace algol {
namespace admin {

  dispatcher::dispatcher(boost::asio::io_service& io_service)
  : strand_(io_service) {
  }

  dispatcher::~dispatcher() {
    events.clear();
  }

  void dispatcher::deliver(const message& evt, bool immediate) {
    events.push_back(message(evt));
    if (!immediate)
      strand_.post( boost::bind( &dispatcher::dispatch, this ) );
    else
      dispatch();
  }

  void dispatcher::dispatch() {
    assert(!events.empty());
    message evt(events.front());
    events.pop_front();

    msg_handlers_t::const_iterator handlers = msg_handlers_.find(evt.uid);
    std::vector<msg_handler_t>::const_iterator handler;
    if (handlers != msg_handlers_.end())
      for (handler = handlers->second.begin();
           handler != handlers->second.end();
           ++handler)
        (*handler)( evt );

    handlers = msg_handlers_.find("Unassigned");
    if (handlers != msg_handlers_.end())
      for (handler = handlers->second.begin();
           handler != handlers->second.end();
           ++handler)
        (*handler)( evt );

  }

  void dispatcher::unbind(message_uid uid)
  {
    assert(msg_handlers_.find(uid) != msg_handlers_.end());

    msg_handlers_.find(uid)->second.clear();
  }

  void dispatcher::reset()
  {
    msg_handlers_.clear();
  }

}
}
