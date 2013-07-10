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

#include "algol/algolSH/algolSH.hpp"
#include "algol/utility.hpp"
#include "algol/file_manager.hpp"

using std::cout;
using std::cerr;
using std::cin;

extern char** my_completion(const char*, int, int);
extern void   add_command(const char*);

namespace algol {
namespace admin {
namespace shell {

  algolSH* algolSH::__instance = NULL;

  algolSH& algolSH::singleton() {
    return *__instance;
  }

  algolSH::algolSH(boost::asio::io_service& io_service, config_t &cfg)
  : io_service_(io_service),
    timer_(io_service),
    conn_(),
    running_(false),
    config_(cfg),
    delegate_(nullptr)
  {

    __instance = this;

    register_delegate(new delegate("generic"));

    // start the connection
    {
      conn_.reset(new connection(io_service_, config_.host, config_.port));
      if (!conn_->connect())
        throw std::runtime_error("couldn't connect to server @ " + config_.host + ":" + config_.port);
      conn_->start();
    }

    // load any delegate plugins
    load_delegates();

    // readline's autocompletion shizzle
    rl_attempted_completion_function = my_completion;
    // path autocompletion when tabulation hit
    rl_bind_key('\t', rl_complete);
  }

  algolSH::~algolSH() {
    algol::plugin_manager &plugin_mgr = algol::plugin_manager::singleton();

    conn_->stop();
    conn_.reset();

    plugin_mgr.unload_plugins();
    plugin_mgr.shutdown();

    delete delegates_.find("generic")->second;
    delegates_.clear();
  }

  /* ------------------------------------------------
   * Static Methods / Helpers
   * ------------------------------------------------ */

  algolSH::config_t algolSH::defaults() {
    config_t cfg;
    cfg.target        = "generic";
    cfg.host          = "127.0.0.1";
    cfg.port          = "60400";
    cfg.print_status  = true;
    cfg.interactive   = true;
    cfg.carat         = "%> ";

    return cfg;
  }

  algolSH::config_t const& algolSH::config() const {
    return config_;
  }

  string_t algolSH::colored_text(string_t const& text, color_id color) {
    string_t color_code = "";

    switch (color) {
      case RED:   color_code = "31m"; break;
      case GREEN: color_code = "32m"; break;
      default:    color_code = "31m";
    }

    std::ostringstream s; s << "\033[0;" << color_code << text << "\033[0m";
    return s.str();
  }

  void algolSH::send(message& msg)
  {
    conn_->send(msg);
  }

  bool algolSH::is_interactive() const {
    return config_.interactive;
  }

  /* ------------------------------------------------
   * Bootstrapping routines
   * ------------------------------------------------ */
  void algolSH::load_delegates() {
    algol::plugin_manager &plugin_mgr = algol::plugin_manager::singleton();
    plugin_mgr.init();
    plugin_mgr.load_plugins("/usr/local/share/algolSH/plugins");
  }

  void algolSH::register_delegate(delegate* d) {
    delegates_.insert(std::make_pair(d->target(), d));

    if (!delegate_)
      delegate_ = d;
  }

  delegates_t const& algolSH::delegates() const {
    return delegates_;
  }

  /* ------------------------------------------------
   * Main stuff
   * ------------------------------------------------ */

  void algolSH::run(string_t cmd)
  {
    char                    *input; // C version of the feed (read by readline)
    string_t                line;   // the feed
    std::vector<string_t>   tokens; // the feed broken into tokens split by ' '
    algol::plugin_manager  &plugin_mgr = algol::plugin_manager::singleton();

    plugin_mgr.initialise_plugins();

    // display a soft error if there's no registered delegate for the specified target
    if (delegates_.find(config_.target) == delegates_.end()) {
      cout
        << config_.carat << colored_text("error: ", RED) << "found no registered delegate for '"
        << config_.target << "', using the generic one.\n";

      config_.target = "generic";
    }

    // non-interactive mode, just process the feed and exit
    if (!is_interactive())
    {
      conn_->get_dispatcher().bind("Unassigned", this, &algolSH::eval_and_quit);

      delegate_ = delegates_.find(config_.target)->second;
      delegate_->bind(conn_->get_dispatcher());

      tokens = utility::split(cmd, ' ');
      return delegate_->on_feed(cmd, tokens);
    }

    running_ = true;

    cout  << config_.carat
          << "algol shell engaged, "
          << "type 'help' for help, 'quit' to quit, have fun!\n";

    // populate supported command list
    {
      message help_cmd("help");
      bool    help_populated = false;

      help_cmd.set_property("--brief", "true");

      conn_->get_dispatcher().bind("help", [&](const message& m) {
        tokens = utility::split(m["Data"], ' ');
        for (auto t : tokens) {
          add_command(t.c_str());
        }

        conn_->get_dispatcher().reset();
        help_populated = true;
      });

      send(help_cmd);
      while (!help_populated)
        sleep(5 / 1000.0);
    }

    delegate_ = delegates_.find(config_.target)->second;
    delegate_->bind(conn_->get_dispatcher());
    conn_->get_dispatcher().bind("Unassigned", this, &algolSH::refresh_shell);

    while (running_)
    {
      // inputing...
      input = readline(config_.carat.c_str());

      // eof
      if (!input)
        break;

      // adding the previous input into history
      add_history(input);

      line = string_t(input);
      if (line == "exit" || line == "quit" || line == "q")
        break;

      if (line.empty()) {
        continue;
      }

      tokens = utility::split(line, ' ');

      delegate_->on_feed(line, tokens);
    }

    cout << "bye!\n";
    plugin_mgr.shutdown_plugins();

    quit();
  }

  void algolSH::quit()
  {
    return (void)kill(getpid(), SIGINT);
  }

  void algolSH::eval_and_quit(const message&)
  {
    // delegate_->on_response(m);
    quit();
  }

  void algolSH::refresh_shell(const message&)
  {
    cout << config_.carat;
    fflush(stdout);
  }

}
}
}
