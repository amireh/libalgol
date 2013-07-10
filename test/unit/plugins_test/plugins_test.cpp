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

#include "plugins_test/plugins_test.hpp"
#include <algol/plugin_manager.hpp>

namespace algol {

  plugins_test::plugins_test() : test("plugins") {
  }

  plugins_test::~plugins_test() {
  }

  int plugins_test::run(int, char**) {
    algol::plugin_manager &pmgr = algol::plugin_manager::singleton();

    pmgr.load_plugins("./plugins.txt");
    pmgr.initialise_plugins();
    pmgr.shutdown_plugins();
    pmgr.unload_plugins();

    result_ = passed;
    return result_;
  }


}
