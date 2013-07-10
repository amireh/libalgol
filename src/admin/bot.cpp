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

#include "algol/admin/bot.hpp"
#include "algol/utility.hpp"

namespace algol {
namespace admin {

  bot::bot(connection* in_conn)
  : logger("bot"),
    conn_(in_conn)
  {
    conn_->bot_ = this;

    // populate the "help" list of supported commands
    if (commands_.empty())
    {
      commands_.insert(std::make_pair("help", "displays this help listing"));
    }

    conn_->get_dispatcher().bind("help", this, &bot::on_help);
  }

  bot::~bot() {
    log_->debugStream() << "going down";
    //~ conn_.reset();
    conn_ = NULL;
  }

  connection* bot::conn()
  {
    return conn_;
  }

  void bot::start()
  {
    log_->infoStream()
      << "servicing the master commander mr system administrator from: "
      << conn_->socket().remote_endpoint().address();
  }

  void bot::send(const message& orig_msg) {
    message msg(orig_msg);
    conn_->send(msg);
  }

  void bot::confirm(const message& orig_msg, string_t const& status) {
    message msg(orig_msg);

    msg.feedback = message_feedback::ok;
    msg.set_property("Status", status);

    conn_->send(msg);
  }

  void bot::reject(const message& orig_msg, string_t const& error, message_feedback feedback) {
    message msg(orig_msg);

    msg.feedback = feedback;
    msg.set_property("Error", error);

    conn_->send(msg);
  }

  void bot::on_quit(const message&)
  {
    //~ conn_.reset();
    //~ server::singleton()._admin_close(shared_from_this());
  }

  void bot::on_help(const message& msg)
  {
    //~ std::cout << "admin wants a help list\n";
    std::ostringstream s;
    for (auto pair : commands_) {
      if (!msg.has_property("--brief")) {
        s << "  " << utility::expand(pair.first, 20, ' ');
        s << pair.second << "\n";
      } else {
        s << pair.first << ' ';
      }
    }
    message out(msg);
    out.options = message::no_format;
    out.set_property("Data", s.str());
    //~ out.set_property("commands", "hi");
    conn_->send(out);
  }

  void bot::bind(message_uid const& command, string_t const& desc, std::function<void(const message&)> handler) {
    if (commands_.find(command) != commands_.end()) {
      log_->warnStream() << "command '" << command << "' seems to already be bound! aborting";
      return;
    }

    commands_.insert(std::make_pair(command, desc));
    conn_->get_dispatcher().bind(command, handler);
  }

}
}
