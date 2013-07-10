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

#include "algol/messaging/message.hpp"
#include "algol/messaging/channel.hpp"
#include "algol/log_manager.hpp"
#include "algol/utility.hpp"

namespace algol {

  string_t message::app_id__ = "";
  void message::set_app_id(string_t const& id) {
    app_id__ = id;
  }

  message::message()
  : channel_(nullptr),
    sender_(nullptr)
  {
    set_body("");
    props_.flags = 0;
    props_.delivery_mode = DELIVERY_MODE_TRANSIENT;
    props_.timestamp = 0;
    props_.priority = 0;
    props_.app_id = app_id__;
  }

  message::message(const string_t &body)
  : channel_(nullptr),
    sender_(nullptr)
  {
    set_body(body);

    props_.flags = 0;
    props_.delivery_mode = DELIVERY_MODE_TRANSIENT;
    props_.timestamp = 0;
    props_.priority = 0;
    props_.app_id = app_id__;
  }

  message::message(const message& src) {
    clone(src);
  }

  message& message::operator=(const message& rhs) {
    if (&rhs != this)
      clone(rhs);

    return *this;
  }

  void message::clone(const message& src) {
    body_                   = src.body_;
    channel_                = src.channel_;
    sender_                 = src.sender_;
    props_.flags            = src.props_.flags;
    props_.content_type     = src.props_.content_type;
    props_.content_encoding = src.props_.content_encoding;
    props_.delivery_mode    = src.props_.delivery_mode;
    props_.priority         = src.props_.priority;
    props_.correlation_id   = src.props_.correlation_id;
    props_.reply_to         = src.props_.reply_to;
    props_.message_id       = src.props_.message_id;
    props_.timestamp        = src.props_.timestamp;
    props_.user_id          = src.props_.user_id;
    props_.app_id           = src.props_.app_id;
  }

  message::~message() {
    channel_ = nullptr;
    sender_ = nullptr;
  }

  void message::set_body(const string_t& in_body) {
    body_ = in_body;
  }
  void message::add_to_body(const string_t &str) {
    body_ += str;
  }

  message& message::operator<<(const string_t& str) {
    add_to_body(str);
    return *this;
  }

  string_t& message::body() {
    return body_;
  }

  string_t const& message::body() const {
    return body_;
  }

  channel* message::get_channel() {
    return channel_;
  }
  string_t const& message::get_queue() const {
    return meta_.queue;
  }

  static string_t amqp_bytes_to_string(amqp_bytes_t* bytes) {
    if (bytes == NULL || bytes->bytes == NULL || bytes->len == 0)
      return "";

    return string_t(reinterpret_cast<const char*>(bytes->bytes), bytes->len);
  }

  void message::serialize(amqp_bytes_t* bytes, amqp_basic_properties_t* props) const {

    // serialize the body
    *bytes                  = amqp_cstring_bytes(body_.c_str());

    // the properties
    props->_flags           = props_.flags;
    props->content_type     = amqp_cstring_bytes(props_.content_type.c_str());
    props->content_encoding = amqp_cstring_bytes(props_.content_encoding.c_str());
    props->delivery_mode    = props_.delivery_mode;
    props->priority         = props_.priority;
    props->correlation_id   = amqp_cstring_bytes(props_.correlation_id.c_str());
    props->message_id       = amqp_cstring_bytes(props_.message_id.c_str());
    props->reply_to         = amqp_cstring_bytes(props_.reply_to.c_str());
    props->user_id          = amqp_cstring_bytes(props_.user_id.c_str());
    props->_flags           |= AMQP_BASIC_TIMESTAMP_FLAG;
    props->timestamp        = time(NULL);
    props->_flags           |= AMQP_BASIC_APP_ID_FLAG;
    props->app_id           = amqp_cstring_bytes(app_id__.c_str());

    // serialize the headers
    if (!headers_.empty()) {
      amqp_table_entry_t *entries = new amqp_table_entry_t[headers_.size()];
      amqp_table_t headers;
      int hidx = -1;
      for (auto h : headers_) {
        entries[++hidx].key = amqp_cstring_bytes(h.key.c_str());
        entries[hidx].value.kind = AMQP_FIELD_KIND_UTF8;
        entries[hidx].value.value.bytes = amqp_cstring_bytes(h.value.c_str());
      }
      headers.num_entries = headers_.size();
      headers.entries = entries;

      ALGOL_LOG->debugStream() << "serialized " << headers_.size() << " headers";

      props->headers  = headers;
      props->_flags   |= AMQP_BASIC_HEADERS_FLAG;
    }

    return;
  }

  void message::deserialize(amqp_basic_properties_t* props) {
    props_.flags            = props->_flags;
    if (props->_flags & AMQP_BASIC_CONTENT_TYPE_FLAG)
      props_.content_type     = amqp_bytes_to_string(&props->content_type);

    if (props->_flags & AMQP_BASIC_CONTENT_ENCODING_FLAG)
      props_.content_encoding = amqp_bytes_to_string(&props->content_encoding);

    props_.delivery_mode    = props->delivery_mode;
    props_.priority         = props->priority;

    if (props->_flags & AMQP_BASIC_CORRELATION_ID_FLAG)
      props_.correlation_id   = amqp_bytes_to_string(&props->correlation_id);

    if (props->_flags & AMQP_BASIC_REPLY_TO_FLAG)
      props_.reply_to         = amqp_bytes_to_string(&props->reply_to);

    if (props->_flags & AMQP_BASIC_MESSAGE_ID_FLAG)
      props_.message_id       = amqp_bytes_to_string(&props->message_id);

    props_.timestamp        = props->timestamp;

    if (props->_flags & AMQP_BASIC_USER_ID_FLAG)
      props_.user_id          = amqp_bytes_to_string(&props->user_id);

    if (props->_flags & AMQP_BASIC_APP_ID_FLAG)
      props_.app_id           = amqp_bytes_to_string(&props->app_id);

    if (props->_flags & AMQP_BASIC_HEADERS_FLAG) {
      int nr_headers = props->headers.num_entries;
      int hidx;
      ALGOL_LOG->debugStream() << "deserializing " << nr_headers << " headers";
      for (hidx = 0; hidx < nr_headers; ++hidx) {
        // as we only support string headers, there's no need to type check
        string_t key = amqp_bytes_to_string(&props->headers.entries[hidx].key);
        string_t value = amqp_bytes_to_string(&props->headers.entries[hidx].value.value.bytes);

        headers_.push_back({ key, value });
      }
    }
  }

  void message::set_content_type(const string_t& prop) {
    props_.flags |= AMQP_BASIC_CONTENT_TYPE_FLAG;
    props_.content_type = prop;
  }
  void message::set_content_encoding(string_t const& prop) {
    props_.flags |= AMQP_BASIC_CONTENT_ENCODING_FLAG;
    props_.content_encoding = prop;
  }
  void message::set_delivery_mode(uint8_t prop) {
    props_.flags |= AMQP_BASIC_DELIVERY_MODE_FLAG;
    props_.delivery_mode = prop;
  }
  void message::set_priority(uint8_t prop) {
    props_.flags |= AMQP_BASIC_PRIORITY_FLAG;
    props_.priority = prop;
  }
  void message::set_correlation_id(string_t const& prop) {
    props_.flags |= AMQP_BASIC_CORRELATION_ID_FLAG;
    props_.correlation_id = prop;
  }
  void message::set_message_id(string_t const& prop) {
    props_.flags |= AMQP_BASIC_MESSAGE_ID_FLAG;
    props_.message_id = prop;
  }

  void message::set_reply_to(string_t const& prop) {
    props_.flags |= AMQP_BASIC_REPLY_TO_FLAG;
    props_.reply_to = prop;
  }
  void message::set_timestamp(uint64_t prop) {
    props_.flags |= AMQP_BASIC_TIMESTAMP_FLAG;
    props_.timestamp = prop;
  }
  void message::set_user_id(string_t const& prop) {
    props_.flags |= AMQP_BASIC_USER_ID_FLAG;
    props_.user_id = prop;
  }

  string_t const& message::get_content_type() const {
    return props_.content_type;
  }
  string_t const& message::get_content_encoding() const {
    return props_.content_encoding;
  }
  uint8_t message::get_delivery_mode() {
    return props_.delivery_mode;
  }
  uint8_t message::get_priority() {
    return props_.priority;
  }
  string_t const& message::get_correlation_id() const {
    return props_.correlation_id;
  }
  string_t const& message::get_reply_to() const {
    return props_.reply_to;
  }
  uint64_t message::get_timestamp() {
    return props_.timestamp;
  }
  string_t const& message::get_user_id() const {
    return props_.user_id;
  }
  string_t const& message::get_app_id() const {
    return props_.app_id;
  }
  string_t const& message::get_message_id() const {
    return props_.message_id;
  }


  void message::dump(std::ostream& s) const {
    s << dump_str();
  }

  void message::dump(log4cpp::CategoryStream s) const {
    s << dump_str();
  }

  string_t message::dump_str() const {
    std::ostringstream s;

    auto print_entry = [&s](string_t const& key, string_t const& value) {
      s << key << ": " << value << '\n';
    };
    auto print_entry_d = [&s](string_t const& key, int value) {
      s << key << ": " << value << '\n';
    };

    print_entry("Exchange", channel_ ? channel_->id() : "N/A");
    print_entry("Queue", meta_.queue);
    print_entry("--", "--");

    print_entry("Body", body_);
    print_entry("Content-Type", props_.content_type);
    print_entry("Content-Encoding", props_.content_encoding);
    print_entry_d("Delivery-Mode", props_.delivery_mode);
    print_entry_d("Priority", props_.priority);
    print_entry("Correlation-ID", props_.correlation_id);
    print_entry("Reply-To", props_.reply_to);
    print_entry("Message-ID", props_.message_id);
    print_entry_d("Timestamp", props_.timestamp);
    print_entry("User-ID", props_.user_id);
    print_entry("App-ID", props_.app_id);
    for (auto h : headers_)
      print_entry("Header[" + h.key + "]", h.value);

    return s.str();
  }

  void message::set_header(string_t const& key, string_t const& value) {
    headers_.push_back({ key, value });
  }

  // void message::set_header(string_t const& key, bool value) {
  //   set_header(key, utility::stringify(value));
  // }

  void message::set_header(string_t const& key, int value) {
    set_header(key, utility::stringify(value));
  }

} // end of namespace algol
