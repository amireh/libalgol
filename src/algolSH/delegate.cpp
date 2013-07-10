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

#include "algol/algolSH/delegate.hpp"
#include "algol/algolSH/algolSH.hpp"

using std::cout;
using std::cerr;
using std::cin;

namespace algol {
namespace admin {
namespace shell {

  void delegate::send(message& msg)
  {
    algolSH::singleton().send(msg);
  }

  void delegate::bind( admin::dispatcher& dispatcher ) {
    dispatcher.bind("Unassigned", this, &delegate::on_response);
  }

  void delegate::on_feed(string_t const&, std::vector<string_t> const& tokens)
  {
    if (tokens.size() < 1) {
      cout << "invalid command\n";
      return;
    }

    message outbound(tokens.front());
    send(outbound);
  }

  void delegate::on_response(const message& m) {
    bool printing_status = algolSH::singleton().config().print_status;
    string_t prefix = algolSH::singleton().is_interactive() ? " -> " : "algolSH: ";

    if (m.has_option(message::no_format)) {
      cout << '\n' << m["Data"] << '\n';
    }
    else if (m.feedback != message_feedback::ok && m.has_property("Error")) {
      // colorize output
      cout << prefix << m["Error"];

      if (printing_status)
       cout << " [ \033[0;31mFAILED\033[0m ]";;

      cout << '\n';
    }
    else if (m.has_property("Status")) {
      // colorize output
      cout << prefix << m["Status"];

      if (printing_status)
        cout << " [ \033[0;32mOK\033[0m ]";

      cout << '\n';
    }
    else {
      cout << "Unknown message format, dumping: \n";
      m.dump(cout);
    }
  }

}
}
}
