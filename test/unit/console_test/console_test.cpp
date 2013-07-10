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

#include "console_test/console_test.hpp"
#include "algol/admin/console.hpp"

namespace algol {

  namespace admin {
    class my_bot : public bot {
    public:
      inline void on_svclist(const message&) {
        std::cout << "svclist\n";
      }

      inline my_bot(connection* c) : bot(c) {
        bind("svclist", "foobar", boost::bind(&my_bot::on_svclist, this, _1));
      }

      inline virtual ~my_bot() {

      }
    };
  }

  console_test::console_test() : test("console", false) {
  }

  console_test::~console_test() {
  }

  int console_test::run(int, char**) {

    try
    {
      // block all signals for background thread
      sigset_t new_mask;
      sigfillset(&new_mask);
      sigset_t old_mask;
      pthread_sigmask(SIG_BLOCK, &new_mask, &old_mask);

      admin::console *c = new admin::console(io_service_);
      c->register_bot_maker([](admin::connection* c) -> admin::my_bot* { return new admin::my_bot(c); });
      c->launch();

      boost::thread w(boost::bind(&boost::asio::io_service::run, &io_service_));

      result_ = passed;

      // restore previous signals
      pthread_sigmask(SIG_SETMASK, &old_mask, 0);

      // wait for signal indicating time to shut down
      sigset_t wait_mask;
      sigemptyset(&wait_mask);
      sigaddset(&wait_mask, SIGINT);
      sigaddset(&wait_mask, SIGQUIT);
      sigaddset(&wait_mask, SIGTERM);
      pthread_sigmask(SIG_BLOCK, &wait_mask, 0);
      int sig = 0;
      sigwait(&wait_mask, &sig);

      stop();
      c->shutdown();
      delete c;
      w.join();
    }
    catch (std::exception& e)
    {
      std::cout << "console_test: " << e.what() << "\n";
    }

    return result_;
  }

  void console_test::main(int, char**) {

  }

}
