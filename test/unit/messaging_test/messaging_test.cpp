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

#include "messaging_test/messaging_test.hpp"
#include "algol/messaging/station.hpp"
#include "algol/messaging/message.hpp"
#include "algol/utility.hpp"
#include "algol/file_manager.hpp"
#include <boost/thread.hpp>

namespace algol {

  static bool      is_listener = false;
  static int       nr_requests = 1;
  static int       nr_threads = 1;
  static int       sleep_sec = 10;
  static string_t  exchange = "test_exchange";
  static string_t  queue = "test_queue";
  static string_t  msg = "Hello World!";
  static string_t  data_path = "";
  static bool      accepting = true;
  static string_t  app_id = algol_app().fqn;

  messaging_test::messaging_test()
  : test("messaging"),
    communicator()
  {
    // subscribe("test_exchange");
  }

  messaging_test::~messaging_test() {
  }

  int messaging_test::run(int argc, char** argv) {

    if (argc > 1) {
      for (int i = 1; i < argc; ++i) {
        string_t arg(argv[i]);

        if (arg == "-l") {
          is_listener = true;
          log_->infoStream() << "will be listening to queue 'test_queue' on exchange 'test_exchange'";
        }
        else if (arg == "--batch") {
          if (argc == i) {
            log_->errorStream() << "invalid argument '--batch', missing parameter (number of requests)";
            return failed;
          }

          nr_requests = utility::convertTo<int>(argv[++i]);
          if (nr_requests == 0)
            nr_requests = 1;
        }
        else if (arg == "-c") {
          if (argc == i) {
            log_->errorStream() << "invalid argument '-c', missing parameter (concurrency level)";
            return failed;
          }

          nr_threads = utility::convertTo<int>(argv[++i]);
          if (nr_threads == 0)
            nr_threads = 1;
        }
        else if (arg == "-m") {
          if (argc == i) {
            log_->errorStream() << "invalid argument '-m', missing parameter (message contents)";
            return failed;
          }

          msg = argv[++i];
        }
        else if (arg == "-e") {
          if (argc == i) {
            log_->errorStream() << "invalid argument '-e', missing parameter (exchange name)";
            return failed;
          }

          exchange = argv[++i];
        }
        else if (arg == "-q") {
          if (argc == i) {
            log_->errorStream() << "invalid argument '-q', missing parameter (queue name)";
            return failed;
          }

          queue = argv[++i];
        }
        else if (arg == "-s") {
          if (argc == i) {
            log_->errorStream() << "invalid argument '-s', missing parameter (seconds to sleep)";
            return failed;
          }

          sleep_sec = utility::convertTo<int>(argv[++i]);
        }
        else if (arg == "--app-id") {
         if (argc == i) {
            log_->errorStream() << "invalid argument '--app-id', missing parameter (id)";
            return failed;
          }

          app_id = argv[++i];
        }
        else if (arg == "-d") {
          if (argc == i) {
            log_->errorStream() << "invalid argument '-d', missing parameter (path to datafile)";
            return failed;
          }

          data_path = argv[++i];
        }
        else if (arg == "-a") {
          if (argc == i) {
            log_->errorStream() << "invalid argument '-a', missing parameter (app id)";
            return failed;
          }

          app_id = argv[++i];
        }
      }
    }

    if (!is_listener) {

      if (!data_path.empty()) {
        bool failed = false;
        bool res = false;
        try {
          res = file_manager::singleton().get_resource(data_path, msg);
        } catch(std::exception &e) {
          failed = true;
        }
        if (failed || !res) {
          msg = "Hello World!";
          log_->errorStream() << "resource located at " << data_path << " could not be read, resetting message to 'Hello World!'";
        }
      }

      subscribe(exchange);

      boost::thread_group workers;

      message m(msg);
      m.set_content_type("text/plain");
      m.set_header("Foo", "Bar");
      m.set_header("Zoo", 5);
      m.set_header("Moo", "true");
      // m.set_app_id(app_id);
      // m.set_content_encoding("UTF-8");
      int nr_requests_per_thread = (int)(nr_requests / nr_threads);

      log_->infoStream() << "Exchange: " << exchange;
      log_->infoStream() << "Queue: " << queue;
      log_->infoStream() << "Body: " << msg;
      log_->infoStream() << "Concurrency Level = " << nr_threads;

      log_->debugStream() << "Message dump:";
      m.dump(std::cout);

      for (int i = 0; i < nr_threads; ++i) {
        workers.create_thread([&, i, nr_requests_per_thread]() -> void {
          log_->infoStream() << "Worker[" << i << "]: sending " << nr_requests_per_thread << " requests.";
          log_->debugStream() << "Message dump:";
          for (int x = 0; x < nr_requests_per_thread; ++x) {
            send(m, exchange, queue);
          }
        });
      }

      sleep(nr_requests_per_thread / 10);

      workers.join_all();

    } else {
      log_->infoStream() << "Accepting messages for " << sleep_sec << " seconds";
      if (!subscribe(exchange, queue))
        return failed;
      // if (!subscribe(exchange, queue + "_2"))
      //   return failed;
      // if (!subscribe(exchange + "_2", queue))
      //   return failed;

      while (accepting) {
        sleep(150 / 1000.0);
      }
      result_ = passed;
      // sleep(sleep_sec);
    }

    return result_;
  }

  void messaging_test::on_message_received(const message& msg) {
    if (msg.body() == "quit") {
      log_->infoStream() << "quitting";
      accepting = false;
      return;
    }

    std::cout << "Message from (" << msg.get_queue() << "): '\n";
    fflush(stdout);

    if (!data_path.empty()) {
      std::cout << "storing in " << data_path << "\n";

      std::ofstream fh;
      fh.open(data_path);
      if (fh.is_open() && fh.good())
        fh << msg.body();
      return;
    }

    //std::cout << msg.body();
    //std::cout << "'\n";
    msg.dump(std::cout);
  }

  void messaging_test::on_message_sent(const message&, comm_rc) {

  }

}
