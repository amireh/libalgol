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
#include "algol/messaging/message.hpp"
#include "algol/utility.hpp"

namespace algol {

  channel::consumer::consumer(channel* c, const string_t &queue)
  : logger(string_t("Channel[" + c->id() + "][" + queue + "]").c_str()),
    c_(c),
    queue_(queue)
  {

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
      throw connection_error("Opening channel " + c_->id());

    amqp_exchange_declare(conn_, 1, amqp_cstring_bytes(c_->id().c_str()), amqp_cstring_bytes("direct"), c_->is_passive_, c_->is_durable_, amqp_empty_table);
    if (amqp_get_rpc_reply(conn_).reply_type != AMQP_RESPONSE_NORMAL) {
      std::ostringstream s;
      s << "Declaring exchange [" << c_->id()
        << "]: durable? " << c_->is_durable_
        << ", passive?" << c_->is_passive_;
      throw connection_error(s.str());
    }

  }

  channel::consumer::~consumer() {
    c_ = nullptr;
  }

  void channel::consumer::stop() {
    amqp_channel_close(conn_, 1, AMQP_REPLY_SUCCESS);
    amqp_connection_close(conn_, AMQP_REPLY_SUCCESS);
    amqp_destroy_connection(conn_);
  }

  void channel::consumer::consume() {
    // acceptors_.create_thread([&]() -> void {
      amqp_bytes_t queuename;

      // declare the queue
      {
        amqp_queue_declare_ok_t *r =
          amqp_queue_declare(conn_, 1, amqp_cstring_bytes((queue_ + "_queue").c_str()), c_->is_passive_, c_->is_durable_, 0, 0, amqp_empty_table);

        if (amqp_get_rpc_reply(conn_).reply_type != AMQP_RESPONSE_NORMAL)
          throw connection_error("Declaring queue");

        queuename = amqp_bytes_malloc_dup(r->queue);
      }

      // bind the queue
      amqp_queue_bind(conn_, 1, queuename, amqp_cstring_bytes(c_->id().c_str()), amqp_cstring_bytes(queue_.c_str()), amqp_empty_table);
      if (amqp_get_rpc_reply(conn_).reply_type != AMQP_RESPONSE_NORMAL)
        throw connection_error("Binding queue");

      amqp_basic_consume(conn_, 1, queuename, amqp_empty_bytes, 0, 1, 0, amqp_empty_table);
      if (amqp_get_rpc_reply(conn_).reply_type != AMQP_RESPONSE_NORMAL)
        throw connection_error("Consuming");

      amqp_bytes_free(queuename);

      log_->infoStream() << "Consuming " << queue_;
      {
        amqp_frame_t frame;
        int result;

        amqp_basic_deliver_t *d;
        amqp_basic_properties_t *p;
        size_t body_target;
        size_t body_received;
        string_t body;

        bool accepting = true;

        while (accepting) {
          // std::cout << "Wait #1\n";
          amqp_maybe_release_buffers(conn_);
          result = amqp_simple_wait_frame(conn_, &frame);
          // printf("Result %d\n", result);
          if (result < 0)
            break;

          // printf("Frame type %d, channel %d\n", frame.frame_type, frame.channel);
          if (frame.frame_type != AMQP_FRAME_METHOD)
            continue;

          // printf("Method %s\n", amqp_method_name(frame.payload.method.id));
          if (frame.payload.method.id == AMQP_CHANNEL_CLOSE_OK_METHOD) {
            log_->infoStream() << "acceptor: channel seems to be closed";
            amqp_channel_close_ok_t close_ok;
            amqp_send_method(conn_, 0, AMQP_CHANNEL_CLOSE_OK_METHOD, &close_ok);
            break;
          }
          else if (frame.payload.method.id == AMQP_CONNECTION_CLOSE_OK_METHOD) {
            log_->infoStream() << "acceptor: connection seems to be closed, stopping";
            accepting = false;
            amqp_connection_close_ok_t close_ok;
            amqp_send_method(conn_, 0, AMQP_CONNECTION_CLOSE_OK_METHOD, &close_ok);
            break;
          }
          else if (frame.payload.method.id != AMQP_BASIC_DELIVER_METHOD)
            continue;

          d = (amqp_basic_deliver_t *) frame.payload.method.decoded;
          // printf("Delivery %u, exchange %.*s routingkey %.*s\n",
          //  (unsigned) d->delivery_tag,
          //  (int) d->exchange.len, (char *) d->exchange.bytes,
          //  (int) d->routing_key.len, (char *) d->routing_key.bytes);

          // std::cout << "Wait #2\n";
          result = amqp_simple_wait_frame(conn_, &frame);
          if (result < 0)
            break;

          if (frame.frame_type != AMQP_FRAME_HEADER) {
            fprintf(stderr, "Expected header!");
            abort();
          }

          p = (amqp_basic_properties_t *) frame.payload.properties.decoded;
          if (p->_flags & AMQP_BASIC_CONTENT_TYPE_FLAG) {
            // printf("Content-type: %.*s\n",
            // (int) p->content_type.len, (char *) p->content_type.bytes);
          }
          // printf("----\n");

          body_target = frame.payload.properties.body_size;
          body_received = 0;
          body.clear();

          while (body_received < body_target) {
            // std::cout << "Wait #3\n";
            result = amqp_simple_wait_frame(conn_, &frame);
            if (result < 0)
              break;

            if (frame.frame_type != AMQP_FRAME_BODY) {
              fprintf(stderr, "Expected body!");
              abort();
            }

            body_received += frame.payload.body_fragment.len;
            body += string_t(reinterpret_cast<const char*>(frame.payload.body_fragment.bytes), frame.payload.body_fragment.len);
            assert(body_received <= body_target);
          }

          // log_->infoStream() << "Expected body size: " << body_target << ", actual: " << body_received;

          string_t queue(reinterpret_cast<const char*>(d->routing_key.bytes), d->routing_key.len);
          message msg(body);

          msg.deserialize(p); // TODO: optimize, there's no need to deserialize the whole message if it's not for us (see below)

          // we will only dispatch the message if it has no recipient, or the recipient is us
          if ( msg.get_app_id() != algol_app().fqn &&
              (msg.get_reply_to().empty() || msg.get_reply_to() == algol_app().fqn) )  {
            msg.meta_.queue = queue_;
            msg.channel_ = c_;
            log_->infoStream() << "dispatching incoming message from (" << msg.get_app_id() << ")";
            c_->dispatch(msg);
          } else {
            if (msg.get_app_id() == algol_app().fqn)
              log_->infoStream() << "rejecting self message.";
            else
              log_->infoStream() << "rejecting message because it's not directed at us (recipient: " << msg.get_reply_to() << ")";
          }

          amqp_maybe_release_buffers(conn_);

          if (body_received != body_target) {
            /* Can only happen when amqp_simple_wait_frame returns <= 0 */
            /* We break here to close the connection */
            break;
          }
        }
      }

    // }); // end of acceptor logic
  }


} // end of namespace algol
