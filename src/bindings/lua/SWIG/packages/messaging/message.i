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

%module lua_algol
%{
  #include "algol/messaging/types.hpp"
  #include "algol/messaging/message.hpp"
%}

namespace algol {

  typedef struct amqp_basic_properties_t_ amqp_basic_properties_t;
  /**
   * @class message
   * @brief
   * A message object that's transmittable by communicators across the platform.
   */
  class channel;
  class communicator;
  class station;
  class message {
  public:

    enum {
      DELIVERY_MODE_TRANSIENT  = 1,
      DELIVERY_MODE_PERSISTENT = 2
    };

    message();
    message(const string_t &body);
    message(const message&);
    message& operator=(const message&);
    virtual ~message();

    /** Assigns a string as the message content */
    void set_body(const string_t&);

    /** Appends a string to the message content */
    void add_to_body(const string_t&);

    /** Convenience interface for appending content to the message */
    message& operator<<(const string_t&);

    /** The message content */
    string_t& body();

    /** An immutable version of the content */
    string_t const& body() const;

    /** The channel through which this message was received */
    channel* get_channel();

    /** The queue to/from which this message was routed */
    string_t const& get_queue() const;

    /**
     * Assigns a Content-Type to the message to indicate its payload type.
     *
     * Example values: "text/plain", "application/xml", "application/json"
     */
    void set_content_type(string_t const&);

    /**
     * Specifies the encoding scheme of the payload. Example value: "UTF-8"
     */
    void set_content_encoding(string_t const&);

    /**
     * Specifies the mode in which this message will be transfered.
     *
     * Transfer modes:
     *  1. Transient: the message will be lost if the broker is disconnected
     *  2. Persistent: the message will be stored and will be restored
     *  in the event of a transport failure
     *
     * @note
     * Assigning a delivery mode to the message does not ensure that it
     * will be delivered that way since that also depends on the type
     * of the queue and exchange it's being sent through.
     */
    void set_delivery_mode(uint8_t);

    /**
     * Assigns a priority level to this message. Accepted values are from [0-9].
     */
    void set_priority(uint8_t);

    /**
     * Sets the correlation application identifier.
     */
    void set_correlation_id(string_t const&);

    /**
     * Specifies a recipient using an arbitrary ID that the application can use to route internally.
     *
     * This is usually useful in 1-to-1 communication when a response is expected for every
     * request.
     */
    void set_reply_to(string_t const&);

    /**
     * Marks the sender of this message using an arbitrary ID which can be used by the recipient to
     * identify the sender and reply if appropriate.
     *
     * @see message::set_reply_to
     */
    void set_user_id(string_t const&);

    /** The type of data stored in the message payload. */
    string_t const& get_content_type() const;

    /** The encoding of the message content. */
    string_t const& get_content_encoding() const;

    /** The delivery mode of this message. */
    uint8_t get_delivery_mode();

    /** The priority with which this message was sent. */
    uint8_t get_priority();

    /** The correlation application ID the communication parties have decided upon (if any). */
    string_t const& get_correlation_id() const;

    /** Is this a reply message? If so, the recipient's ID is stored here. */
    string_t const& get_reply_to() const;

    /** The time (in seconds) at which this message was sent. */
    uint64_t get_timestamp();

    /** The sender's id. */
    string_t const& get_user_id() const;

    /** The sender application's id. */
    string_t const& get_app_id() const;

    /** The automatically-assigned ID of this message. This can be used to track and acknowledge requests. */
    string_t const& get_message_id() const;

    /**
     * Adds a meta-header to the message.
     *
     * Headers are optional and can be used to filter or route at the application level.
     *
     * @note All headers are internally cast to strings and transported that way.
     */
    void set_header(string_t const&key, string_t const&);

    /** Helper for adding an integer-valued header. */
    void set_header(string_t const&key, int);

    void dump(std::ostream&) const;
    void dump(log4cpp::CategoryStream&) const;
    string_t dump_str() const;

  protected:
    friend class channel;
    friend class station;

    channel       *channel_;
    communicator  *sender_; /// a transient field, used internally

    struct meta_t {
      string_t  queue;
    } meta_;

    void set_timestamp(uint64_t);

    /**
     * The application ID is global and is maintained by the station.
     *
     * @remark
     * Format: "[app_name][app_version][host]"; where host is the FQDN of the host machine.
     *
     * @note
     * The application information is acquired via algol::algol_app()
     */
    static void set_app_id(string_t const&);
    void serialize(amqp_bytes_t*, amqp_basic_properties_t*) const;
    void deserialize(amqp_basic_properties_t*);

  private:
    void clone(const message&);

  private:
    static string_t app_id__;
    string_t        body_;

    struct properties_t {
      amqp_flags_t  flags;
      string_t      content_type;
      string_t      content_encoding;
      uint8_t       delivery_mode;
      uint8_t       priority;
      string_t      correlation_id;
      string_t      reply_to;
      string_t      message_id;
      uint64_t      timestamp;
      string_t      user_id;
      string_t      app_id;
    } props_;

    struct header_t {
      string_t      key;
      string_t      value;
    };

    typedef std::vector<header_t> headers_t;
    headers_t headers_;
  };

} // end of namespace algol
