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

#ifndef H_ALGOL_MESSAGING_COMMUNICATOR_H
#define H_ALGOL_MESSAGING_COMMUNICATOR_H

#include "algol/algol.hpp"
#include "algol/logger.hpp"
#include "algol/messaging/types.hpp"
#include <map>

namespace algol {

  /**
   * \addtogroup Messaging
   * @{
   * @class communicator
   * Communicator instances can send and receive messeages to and from pre-registered targets over
   * channels. Communicators and their targets must be uniquely identifiable per-network and not per-instance.
   *
   * A usage example:
   *
   * Your application is called "dakapi", and you have 4 Communicator instances, all 4 will share
   * the same ID, "dakapi", but can communicate over separate channels or shared ones, that are, consecutively,
   * uniquely identifeid. So, one instance, called the Analyzer, can send and receive messages over the "analysis"
   * channel with other communicators who are subscribed to that channel, while another instance, the Butcher,
   * can freely communicate over the "slaughterhouse" channel.
   */
  class channel;
  class message;
  class communicator {
  public:

    communicator();
    communicator& operator=(const communicator& rhs);
    communicator(const communicator& src);
    virtual ~communicator();

    /**
     * Sends a message over the specified channel and queue.
     *
     * @warn
     * Before sending, you should make sure the channel is open by calling
     * communicator::subscribe(channel_id_t).
     */
    void send(const message&, const channel_id_t&, const string_t &queue);

    /**
     * Sends a message over the specified channel and queue.
     */
    void send(const message&, channel*, const string_t &queue);

    /**
     * Subscribe as a publisher to this channel. Note that this subscription
     * is not exclusive; it is OK to subscribe as a listener to the same channel
     * as well.
     *
     * Subscribing as a publisher will guarantee the channel to be in a ready
     * state when this communicator instance wants to publish a message.
     */
    virtual bool subscribe(channel_id_t);

    /**
     * Subscribe as a listener to a queue in a channel. Whenever a message is
     * delivered into that queue, this instance will be notified.
     */
    virtual bool subscribe(channel_id_t, queue_id_t, int durable = 1, int passive = 1);

    /**
     * Will no longer receive messages over the specified channel & queue.
     */
    virtual bool unsubscribe(channel_id_t, queue_id_t);

    /**
     * True if this communicator is subscribed to messages sent or received
     * over this channel & queue.
     */
    virtual bool is_subscribed(channel_id_t, queue_id_t);

  protected:
    friend class channel;

    /**
     * Invoked by a channel when a message is received.
     */
    virtual void on_message_received(const message&);

    /**
     * Invoked by a channel when a message is sent, contains the message and the communication result.
     */
    inline virtual void on_message_sent(const message&, comm_rc) {};

  private:
    void clone(const communicator& src);
  }; // end of communicator class

  /** @} */
} // end of namespace algol

#endif
