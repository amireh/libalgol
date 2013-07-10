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

#include "algol/algol.hpp"
#include "algol/algolSH/algolSH.hpp"
#include "algol/utility.hpp"

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include <pthread.h>
#include <signal.h>
#include <map>

using namespace algol::admin::shell;
using namespace algol;

std::map<std::string, std::string> commands_;

int print_help()
{
  std::cout << "Usage: algolSH TARGET [OPTIONS]\n";
  std::cout << "algolSH: administrative shell for algol back-end components\n";
  std::cout << "       (run with --supported-targets to list the currently supported targets)\n\n";

  commands_.insert(std::make_pair("--help", "displays this help listing"));
  commands_.insert(std::make_pair("--supported-targets", "displays a list of currently supported client targets to connect to"));
  commands_.insert(std::make_pair("-e CMD ", "evaluates the given command and exits (non-interactive mode)"));
  commands_.insert(std::make_pair("-h HOST", "address of the dakapi host to connect to (default: 127.0.0.1)"));
  commands_.insert(std::make_pair("-p PORT", "the port dakapi is running on (default: 60300)"));
  commands_.insert(std::make_pair("--no-status", "command response status will not be displayed (default: on)"));

  std::cout << "Optional arguments:\n";
  for (auto pair : commands_) {
    std::cout << "  " << utility::expand(pair.first, 30, ' ');
    std::cout << pair.second << "\n";
  }
  return 1;
}

int main(int argc, char** argv)
{
  boost::asio::io_service io_service;
  string_t cmd = "";
  algolSH::config_t cfg = algolSH::defaults();
  string_t first_arg = "";

  if (argc < 2)
    return print_help();

  first_arg = string_t(argv[1]);

  if (first_arg == "--supported-targets") {
    algolSH *sh = nullptr;
    try {
      sh = new algolSH(io_service, cfg);
    } catch(std::exception &e) {
    }

    sh->load_delegates();
    int i = 0;
    std::cout << "Supported targets [" << sh->delegates().size() << "]: \n";
    for (auto pair : sh->delegates()) {
      std::cout << "  " << ++i << ". " << pair.first << '\n';
    }

    delete sh;
    return 1;
  }
  else if (argc > 2)
  {
    int i;
    for (i = 2; i < argc; ++i)
    {
      string_t arg(argv[i]);
      if (arg == "-e")
      {
        cfg.interactive = false;

        if (argc <= i+1) {
          std::cerr << "algolSH: missing command to argument '-e'\n";
          return print_help();
        }
        cmd = "";
        for (int j = i + 1; j < argc; ++j)
          cmd = cmd + string_t(argv[j]);
      }

      else if (arg == "-h")
      {
        if (argc <= i+1) {
          std::cerr << "algolSH: missing host address to argument '-h'\n";
          return print_help();
        }

        cfg.host = string_t(argv[++i]);
      }

      else if (arg == "-p")
      {
        if (argc <= i+1) {
          std::cerr << "algolSH: missing port number to argument '-p'\n";
          return print_help();
        }

        cfg.port = string_t(argv[++i]);
      }
      else if (arg == "--no-status")
      {
        cfg.print_status = false;
      }
    }
  }

  cfg.target = first_arg;

  try
  {
    // block all signals for background thread
    sigset_t new_mask;
    sigfillset(&new_mask);
    sigset_t old_mask;
    pthread_sigmask(SIG_BLOCK, &new_mask, &old_mask);

    //~ boost::asio::io_service::work work(io_service);

    algolSH shell(io_service, cfg);
    boost::thread w(boost::bind(&boost::asio::io_service::run, &io_service));
    boost::thread t(boost::bind(&algolSH::run, &shell, cmd));

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

    io_service.stop();
    shell.quit();
    t.join();
  }
  catch (std::exception& e)
  {
    std::cout << "algolSH: " << algolSH::colored_text("error: ", algolSH::RED) << e.what() << "\n";
  }

  return 0;
}
