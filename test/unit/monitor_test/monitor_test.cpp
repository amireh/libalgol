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

#include "monitor_test/monitor_test.hpp"
#include "algol/log_manager.hpp"
#include "algol/monitor.hpp"
#include <cmath>

namespace algol {

  monitor_test::monitor_test() : test("monitor") {
  }

  monitor_test::~monitor_test() {
  }

  int monitor_test::run(int, char**) {

    monitor& mnt = monitor::singleton();

    monitor::stat_id id = mnt.track_avg_stat("conn time");
    mnt.avg_stat(id, pow(1000, 5));
    mnt.avg_stat(id, pow(1000, 10));
    mnt.avg_stat(id, pow(1000, 5));
    mnt.avg_stat(id, pow(1000, 5));
    mnt.avg_stat(id, pow(1000, 5));
    //~ mnt.avg_stat(id, 600);
    //~ mnt.avg_stat(id, 1000000000000000u * 1000000000000000u);
    mnt.avg_stat(id, 3);
    //~ mnt.avg_stat(id, 13);

    std::vector<monitor::cavg_t*> const& avgs = mnt.avg_stat(id);
    std::cout << "There are " << avgs.size() << " averages calculated\n";
    for (auto avg : avgs)
    {
      std::cout << "Avg = " << avg->val << " from " << avg->pop << " entries" << "\n";
    }


    return result_;
  }

}
