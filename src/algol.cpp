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
#include "algol/file_manager.hpp"
#include "algol/log_manager.hpp"
#include "algol/configurator.hpp"
#include "algol/monitor.hpp"
#ifdef ALGOL_MESSAGING
  #include "algol/messaging/station.hpp"
#endif
#include "algol/plugin_manager.hpp"
#include "algol/utility.hpp"

namespace algol {

  static algol_app_t algol_app__;

  void algol_init(string_t name, string_t vmajor, string_t vminor, string_t vpatch, string_t vbuild)
  {

    algol_app__.name          = name;
    algol_app__.hostname      = utility::get_hostname();
    algol_app__.version_major = vmajor;
    algol_app__.version_minor = vminor;
    algol_app__.version_patch = vpatch;
    algol_app__.version_build = vbuild;
    algol_app__.version       = algol_app__.version_major + string_t(".") +
                                 algol_app__.version_minor + string_t(".") +
                                 algol_app__.version_patch + string_t("-") +
                                 algol_app__.version_build;
    algol_app__.fqn           = algol_app__.name + "-" +
                                 algol_app__.version + "@" +
                                 algol_app__.hostname;

    // set up the logger
    file_manager::singleton().resolve_paths();
    log_manager::singleton().init();
    log_manager::singleton().config_.silent = true;
    log_manager::singleton().configure();
    plugin_manager::singleton().init();
    monitor::singleton();
    #ifdef ALGOL_MESSAGING
      station::singleton();
    #endif

    configurator::subscribe(&log_manager::singleton(), "log manager");
  }

  void algol_cleanup()
  {
    ALGOL_LOG->infoStream() << "algol cleaning up";

    #ifdef ALGOL_MESSAGING
      station::singleton().shutdown();
      delete &station::singleton();
    #endif

    delete &monitor::singleton();
    delete &file_manager::singleton();
    plugin_manager::singleton().shutdown();
    delete &plugin_manager::singleton();
    log_manager::singleton().cleanup();
    delete &log_manager::singleton();
  }

  algol_app_t const& algol_app() {
    return algol_app__;
  }
}
