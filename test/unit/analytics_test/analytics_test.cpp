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

#include "analytics_test/analytics_test.hpp"
#include "algol/analytics/sheet.hpp"
#include "algol/analytics/tracker.hpp"

namespace algol {

  class zoo : public analytics::tracker {
  public:
    zoo() {

    }

    virtual ~zoo() {

    }

    void call_herd() {
      assert(ss_);

      sheet()["herd"].inc_stat("Gorillas", 1);
      sheet()["herd"].inc_stat("Zebras", 3);
    }
  };

  class foo : public analytics::tracker {
  public:
    foo() : tracker("foo") {
      z.attach_sheet(ss_);

    }

    virtual ~foo() {
    }

    void kill_zombies() {

      sheet().add_stat("Mongorilla", "Ayaye!");
      sheet()["cousins"]["mozzarela"].add_stat("nakhleh", "mkhallal");
    }

    zoo z;
  protected:
  };



  analytics_test::analytics_test() : test("analytics") {
  }

  analytics_test::~analytics_test() {
  }

  int analytics_test::run(int, char**) {
    foo f;

    f.kill_zombies();
    f.z.call_herd();

    f.sheet().commit();
    f.z.call_herd();
    f.sheet().commit();

    return result_;
  }


}
