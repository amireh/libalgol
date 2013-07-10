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

#include "configurator_test/configurator_test.hpp"
#include <algol/configurator.hpp>

namespace algol {

  configurator_test::configurator_test() : test("configurator") {
  }

  configurator_test::~configurator_test() {
  }

  int configurator_test::run(int, char**) {
    string_t json;
    {
      // case 1
      json = "{ 'foo': 'bar',, }";

      log_->infoStream() << "Testing with JSON '" << json << "'";

      configurator cfg(json);
      cfg.run();
    }

    {
      // case 1
      json = "{ 'foo': 'bar', 'zoo': { 'hadooken' } }";

      log_->infoStream() << "Testing with JSON '" << json << "'";

      configurator cfg(json);
      cfg.run();
    }

    return result_;
  }


}
