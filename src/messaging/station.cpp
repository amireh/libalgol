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

#include "algol/messaging/station.hpp"
#include "algol/messaging/message.hpp"
#include "algol/utility.hpp"

namespace algol {

  station* station::__instance = 0;

  station::station()
  : configurable({"messaging"}),
    logger("station")
  {
    config.host = "localhost";
    config.port = "5672";
    config.username = "guest";
    config.password = "guest";
    config.vhost = "/";

    message::set_app_id(algol_app().fqn);
  }

  station::~station()
  {
  }

  station& station::singleton() {
    if (!__instance)
      __instance = new station();

    return *__instance;
  }

  void station::set_option(string_t const& key, string_t const& value)
  {
    if (key == "host") {
      config.host = value;
    }
    else if (key == "port") {
      config.port = value;
    }
    else if (key == "vhost") {
      config.vhost = value;
    }
    else if (key == "username") {
      config.username = value;
    }else if (key == "password") {
      config.password = value;
    }
    else {
      std::cerr << "unknown station config setting '" << key << "' => '" << value << "', discarding";
    }
  }

  channel* station::get_channel(channel_id_t const& id) {
    channels_t::iterator finder = channels_.find(id);
    return finder == channels_.end() ? nullptr : finder->second;
  }

  channel* station::open_channel(channel_id_t const& id, int durable, int passive) {
    channel *c = get_channel(id);
    if (!c) {
      try {
        c = new channel(id);
        // TODO: error handling
        c->open(durable, passive);
      } catch (connection_error &e) {
        log_->errorStream() << "channel '" << id << "' could not be opened; cause: " << e.what();

        return nullptr;
      }
    }

    channels_.insert(std::make_pair(id, c));

    return c;
  }

  bool station::is_channel_open(channel_id_t const& id) {
    channel* c = get_channel(id);

    if (!c)
      return false;

    return c->is_open();
  }

  bool station::close_channel(channel_id_t const& id) {
    channel *c = get_channel(id);

    if (!c || !c->is_open())
      return false;

    // TODO: error handling
    c->close();

    return true;
  }

  void station::shutdown() {
    // log_->infoStream() << "shutting down " << channels_.size() << " channels";
    for (auto pair : channels_) {
      close_channel(pair.second->id());
      delete pair.second;
    }

    channels_.clear();
  }

  std::list<channel*> station::channels() {
    std::list<channel*> ret;
    for (auto pair : channels_) {
      ret.push_back(pair.second);
    }
    return ret;
  }
} // namespace algol
