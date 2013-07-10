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

#include "algol/messaging/communicator.hpp"
#include "algol/messaging/station.hpp"
#include "algol/messaging/channel.hpp"

namespace algol {

  communicator::communicator() {
    // std::cout << "communicator created\n";
  }

  communicator& communicator::operator=(const communicator& rhs) {
    if (&rhs != this)
      clone(rhs);

    return *this;
  }

  communicator::communicator(const communicator& src) {
    clone(src);
  }

  void communicator::clone(const communicator&) {
  }

  communicator::~communicator() {
    // std::cout << "communicator destroyed\n";

    std::list<channel*> channels(station::singleton().channels());
    for (auto c : channels) {
      c->unsubscribe_all(this);
    }
  }

  bool communicator::subscribe(channel_id_t id) {
    channel *c = station::singleton().open_channel(id);

    return c != nullptr;
  }

  bool communicator::subscribe(channel_id_t id, queue_id_t queue, int durable, int passive) {
    channel *c = station::singleton().open_channel(id, durable, passive);

    if (!c)
      return false;

    c->subscribe(queue, this);

    return true;
  }

  bool communicator::unsubscribe(channel_id_t id, queue_id_t queue) {
    channel *c = station::singleton().open_channel(id);
    if (!c)
      return true;

    return c->unsubscribe(queue, this);
  }

  bool communicator::is_subscribed(channel_id_t id, queue_id_t queue) {
    channel *c = station::singleton().open_channel(id);
    if (!c)
      return false;

    return c->is_subscribed(queue, this);
  }

  void communicator::send(const message& msg, channel* c, const string_t &queue) {
    c->publish(this, msg, queue);
  }

  void communicator::send(const message& msg, const channel_id_t& id, const string_t &queue) {
    channel* c = station::singleton().open_channel(id);
    if (!c) {
      ALGOL_LOG->errorStream() << "attempting to publish over an invalid channel '" << id << '\'';
      return;
    }

    send(msg, c, queue);
  }

  /*void communicator::bind(message_uid const& uid, std::function<void(const message&)> handler) {
    dispatcher_->bind(uid, handler);
  }

  void communicator::unbind(message_uid const& uid) {
    dispatcher_->unbind(uid;)
  }
  */

  void communicator::on_message_received(const message& m) {
     std::cout << "base on_message_received();\n";
  }

} // end of namespace algol
