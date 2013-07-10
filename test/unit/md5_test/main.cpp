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

#include "md5_test/md5_test.hpp"
#include "algol/algol.hpp"
#include "algol/configurator.hpp"

using namespace algol;

int main(int argc, char** argv) {
  algol::configurator::silence();
  algol::log_manager::silence();
  algol_init("test", "0", "1", "0", "a1");

  int rc = 0;
  {
    md5_test my_test;
    my_test.main(argc, argv);
    rc = my_test.run(argc, argv);
    my_test.report(rc);
  }

  algol_cleanup();

  return rc;
}
