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

#ifndef H_ALGOL_MESSAGING_STATION_H
#define H_ALGOL_MESSAGING_STATION_H

#include "algol/algol.hpp"
#include "algol/logger.hpp"
#include "algol/messaging/channel.hpp"

#include <map>

namespace algol {

  /**
   * \addtogroup Messaging
   * @{
   * @class station
   *
   * Manages messaging Channels and provides an interface to open and close them.
   */
  class station : public configurable, public logger {
  public:

    static station& singleton();

    virtual ~station();
    station(const station&) = delete;
    station& operator=(const station&) = delete;

    /* the station's config context is "messaging" */
    struct config_t {
      string_t host;
      string_t port;
      string_t vhost;
      string_t username;
      string_t password;
    } config;

    /**
     * Opens a messaging channel with the requested identifier if it's not already opened
     * that can be used for both publishing and consuming.
     *
     * nullptr will be returned if the channel could not be opened.
     */
    channel* open_channel(channel_id_t const&, int durable = 1, int passive = 1);

    /**
     * Closes the messaging channel with the requested identifier.
     *
     * @return true when the channel was closed cleanly, false otherwise
     */
    bool close_channel(channel_id_t const&);

    /**
     * Returns true if a channel with the given identifier exists and is open
     */
    bool is_channel_open(channel_id_t const&);

    /**
     * Closes all open messaging channels and cleans up any used resources.
     */
    void shutdown();

    std::list<channel*> channels();

    virtual void set_option(const string_t&, const string_t&);

  private:
    explicit station();
    static station* __instance;

    channel* get_channel(channel_id_t const&);

    typedef std::map<channel_id_t, channel*> channels_t;
    channels_t channels_;
  };

  /** @} */
} // namespace algol

#endif // H_ALGOL_MESSAGING_STATION_H
