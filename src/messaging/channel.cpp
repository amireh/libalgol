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

#include "algol/messaging/channel.hpp"
#include "algol/messaging/station.hpp"
#include "algol/messaging/communicator.hpp"
#include "algol/messaging/message.hpp"
#include "algol/utility.hpp"

namespace algol {

  typedef boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> scoped_lock;

  channel::channel(channel_id_t id)
  : id_(id),
    logger(("Channel[" + id + "]").c_str()),
    open_(false),
    is_durable_(0),
    is_passive_(0)
  {
  }

  channel::~channel() {
  }

  channel_id_t const& channel::id() const {
    return id_;
  }

  // channel::subscribers_t const& channel::subscribers() const {
  //   return subscribers_;
  // }

  void channel::publish(const communicator*, const message& m, const string_t &queue) {
    scoped_lock lock(publishing_mtx_);

    amqp_bytes_t bytes;
    amqp_basic_properties_t props;
    message msg(m);
    msg.serialize(&bytes, &props);
    msg.channel_ = this;
    msg.meta_.queue = queue;

    log_->debugStream() << "publishing message:";
    msg.dump(log_->debugStream());

    amqp_basic_publish(conn_,
      1,
      amqp_cstring_bytes(id_.c_str()),
      amqp_cstring_bytes(queue.c_str()),
      0,
      0,
      &props,
      bytes);
  }

  void channel::open(int durable, int passive) {
    if (is_open()) {
      log_->warnStream() << "attempting to open an already open channel!";
      return;
    }

    is_durable_ = durable;
    is_passive_ = passive;

    const char *host  = station::singleton().config.host.c_str();
    int         port  = utility::convertTo<int>(station::singleton().config.port);
    const char *un    = station::singleton().config.username.c_str();
    const char *pw    = station::singleton().config.password.c_str();
    const char *vhost = station::singleton().config.vhost.c_str();

    conn_ = amqp_new_connection();

    if ((socket_ = amqp_open_socket(host, port)) < 0)
      throw connection_error("Opening socket with RabbitMQ broker");

    amqp_set_sockfd(conn_, socket_);

    if (amqp_login(conn_, vhost, 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, un, pw).reply_type != AMQP_RESPONSE_NORMAL)
      throw connection_error("Logging in to RabbitMQ");

    // open the broker connection
    amqp_channel_open(conn_, 1);
    if (amqp_get_rpc_reply(conn_).reply_type != AMQP_RESPONSE_NORMAL)
      throw connection_error("Opening channel " + id_);

    amqp_exchange_declare(conn_, 1, amqp_cstring_bytes(id_.c_str()), amqp_cstring_bytes("direct"), passive, durable, amqp_empty_table);
    if (amqp_get_rpc_reply(conn_).reply_type != AMQP_RESPONSE_NORMAL)
      throw connection_error("Declaring exchange");

    log_->infoStream() << "open";
    open_ = true;
  }

  void channel::close() {
    if (!is_open()) {
      log_->warnStream() << "attempting to close an already closed channel!";
      return;
    }

    log_->infoStream() << "closing";
    for (auto pair : subscribers_) {
      pair.second.clear();
    }
    subscribers_.clear();

    while (!consumers_.empty()) {
      consumers_.back()->stop();
      delete consumers_.back();
      consumers_.pop_back();
    }

    acceptors_.join_all();

    amqp_channel_close(conn_, 1, AMQP_REPLY_SUCCESS);
    amqp_connection_close(conn_, AMQP_REPLY_SUCCESS);
    amqp_destroy_connection(conn_);

    open_ = false;

    log_->infoStream() << "closed";
  }

  void channel::accept(const string_t &queue) {
    consumers_.push_back(new consumer(this, queue));
    acceptors_.create_thread(boost::bind(&channel::consumer::consume, consumers_.back()));
  }

  bool channel::is_open() const {
    return open_;
  }

  bool channel::subscribe(queue_id_t const& queue, communicator* c) {
    scoped_lock lock(subscription_mtx_);

    if (is_subscribed(queue, c))
      return false;

    if (subscribers_.find(queue) == subscribers_.end()) {
      subscribers_.insert(std::make_pair(queue, queue_subscribers_t()));
      accept(queue);
    }

    subscribers_.find(queue)->second.push_back(c);

    return true;
  }

  bool channel::unsubscribe(queue_id_t const& queue, communicator* c) {
    scoped_lock lock(subscription_mtx_);

    if (!is_subscribed(queue, c))
      return false;

    queue_subscribers_t& subs = subscribers_.find(queue)->second;

    for (queue_subscribers_t::iterator i = subs.begin(); i != subs.end(); ++i)
      if ((*i) == c) {
        subs.erase(i);
        return true;
      }

    return false;
  }

  void channel::unsubscribe_all(communicator* c) {
    // scoped_lock lock(subscription_mtx_);

    for (auto pair : subscribers_) {
      unsubscribe(pair.first, c);
    }
  }

  bool channel::is_subscribed(queue_id_t const& queue, communicator* c) const {
    subscribers_t::const_iterator finder = subscribers_.find(queue);
    if (finder == subscribers_.end())
      return false;

    for (auto s : finder->second)
      if (s == c)
        return true;

    return false;
  }

  void channel::dispatch(const message& msg) {
    scoped_lock lock(subscription_mtx_);

    subscribers_t::iterator finder = subscribers_.find(msg.get_queue());
    if (finder == subscribers_.end()) {
      log_->warnStream() << "no subscribers found for queue " << msg.get_queue() <<  ", discarding message";
      return;
    }

    log_->debugStream() << "dispatching message to " << finder->second.size() << " subscribers";
    for (auto s : finder->second) {
      s->on_message_received(msg);
    }
    log_->debugStream() << "done!";
  }

  int channel::__socket() {
    return socket_;
  }

  amqp_connection_state_t& channel::__connection() {
    return conn_;
  }
} // end of namespace algol
