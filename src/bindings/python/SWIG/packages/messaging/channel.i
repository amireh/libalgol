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

%module algol_py
%{
  #include "algol/messaging/channel.hpp"
  #include "algol/messaging/types.hpp"
%}

namespace algol {

  class communicator;
  class message;
  class station;
  /**
   * @class channel
   * @brief
   * Channels are the railway for messages sent between communicator instances
   * across the network and applications.
   *
   * Channel instances have a unique identifier throughout the network that is
   * composed of the application ID, and the channel name.
   *
   * Communicator instances can not send messages on their own, only through
   * channels. When an inbound message is queued within a channel, all
   * subscribers are notified.
   */
  class channel : public logger {
  public:
    typedef std::list<communicator*> queue_subscribers_t;
    typedef std::map<queue_id_t, queue_subscribers_t> subscribers_t;

    channel(channel_id_t);
    virtual ~channel();

    channel_id_t const& id() const;

    /**
     * Adds a communicator instance to the party that will be notified
     * whenever a message is delivered to the specified queue.
     */
    virtual bool subscribe(queue_id_t const&, communicator*);

    /**
     * Removes the communicator subscription from a queue.
     */
    virtual bool unsubscribe(queue_id_t const&, communicator*);

    /**
     * Removes the communicator instance from all queue subscriptions.
     */
    virtual void unsubscribe_all(communicator*);

    virtual bool is_subscribed(queue_id_t const&, communicator*) const;

    // virtual subscribers_t const& subscribers() const;

    /**
     * Is this channel open for publishing messages?
     */
    bool is_open() const;

    /**
     * Publishes the given message to the destination queue in this channel.
     */
    void publish(const communicator*, const message&, const string_t &queue);

    /** The underlying connection object */
    amqp_connection_state_t& __connection();

    /** The underlying socket fd */
    int __socket();

  protected:
    /** dispatches the received message to all subscribed communicators */
    void dispatch(const message&);

    friend class station;

    /** establishes a connection with the broker and opens a publishing channel */
    void open();

    /**
     * Launches a thread that will listen to the given queue for any messages
     * and dispatch them to the subscribed party members.
     */
    void accept(string_t const& queue);

    /** destroys all links with the broker and disconnects from the messaging platform */
    void close();

  private:
    channel_id_t  id_;
    bool          open_;
    subscribers_t subscribers_;

    amqp_connection_state_t conn_;
    int                     socket_;

    boost::thread_group acceptors_;
    boost::interprocess::interprocess_mutex subscription_mtx_;
    boost::interprocess::interprocess_mutex publishing_mtx_;

  private:
    class consumer : public logger {
    public:
      consumer(channel* c, string_t const& queue);
      virtual ~consumer();

      void consume();
      void stop();
    private:
      amqp_connection_state_t conn_;
      int                     socket_;
      channel                 *c_;
      string_t                queue_;
    };

  private:
    friend class consumer;

    typedef std::vector<consumer*> consumers_t;
    consumers_t consumers_;
  };

} // end of namespace algol
