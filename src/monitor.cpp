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

#include "algol/monitor.hpp"
#include "algol/utility.hpp"

#define POW_2_64 ((double)(1 << 31) * (double)(1 << 31) * 4)
#ifndef UINT64_MAX
  #define UINT64_MAX 18446744073709551615u
#endif

namespace algol {

  monitor* monitor::__instance = 0;
  uint32_t monitor::__guid = 0;

  monitor::monitor()
  : logger("monitor")
  {
  }

  monitor::~monitor()
  {
    for (auto pair : avg_stats_)
    {
      while (!pair.second.empty())
      {
        delete pair.second.back();
        pair.second.pop_back();
      }
    }

  }

  monitor& monitor::singleton() {
    if (!__instance)
      __instance = new monitor();

    return *__instance;
  }

  monitor::stats_t const& monitor::stats() const
  {
    return stats_;
  }
  monitor::avg_stats_t const& monitor::avg_stats() const
  {
    return avg_stats_;
  }

  monitor::stat_val monitor::stat(stat_id id)
  {
    if (stats_.find(id) != stats_.end()) return stats_[id];
    if (avg_stats_.find(id) != avg_stats_.end()) return avg_stats_[id].back()->val;

    throw std::runtime_error(
    "algol::monitor::stat(): requested stat '" + utility::stringify(id) + "' does not exist!");
  }

  std::vector<monitor::cavg_t*> const& monitor::avg_stat(stat_id id)
  {
    if (avg_stats_.find(id) != avg_stats_.end()) return avg_stats_[id];

    throw std::runtime_error(
    "algol::monitor::avg_stat(): requested average stat '" + utility::stringify(id) + "' does not exist!");
  }

  monitor::stat_id monitor::track_stat(string_t const& name)
  {
    for (auto pair : stat_names_)
    {
      if (pair.second == name)
        return 0;
    }

    stat_id id = ++__guid;;
    stats_.insert(std::make_pair(id, 0));
    stat_names_.insert(std::make_pair(id, name));
    return id;
  }

  monitor::stat_id monitor::track_avg_stat(string_t const& name)
  {
    for (auto pair : stat_names_)
    {
      if (pair.second == name)
        return 0;
    }

    stat_id id = ++__guid;

    std::vector<cavg_t*> avgs;
    cavg_t *avg = new cavg_t();
    avg->pop = 0;
    avg->val = 0;
    avgs.push_back(avg);
    avg = nullptr;
    avg_stats_.insert(std::make_pair(id, avgs));

    stat_names_.insert(std::make_pair(id, name));
    return id;
  }

  string_t const& monitor::to_string(stat_id id)
  {
    return stat_names_[id];
  }

  void monitor::inc_stat(stat_id id)
  {
    stats_[id] += 1;
  }

  void monitor::dec_stat(stat_id id)
  {
    stats_[id] -= 1;
  }

  void monitor::avg_stat(stat_id id, uint64_t entry)
  {
    cavg_t* avg = avg_stats_[id].back();
    assert(avg);

    // check for a possible overflow and guard against it
    // note: 18446744073709551615 == (1<<64)-1 (uint64_t)
    if ((avg->val*avg->pop) + entry < avg->val*avg->pop) {
      log_->warnStream()
      << "an integer overflow caught in average stat "
      << to_string(id) << " => {#" << avg->pop << ", " << avg->val << ", " << entry << "}";

      avg = new cavg_t();
      avg->pop = 0;
      avg->val = 0;
      avg_stats_[id].push_back(avg);
    }

    // calculate the average, for more info see:
    //  http://en.wikipedia.org/wiki/Moving_average#Cumulative_moving_average
    std::ostringstream s;
    s << "adding " << entry << " to " << avg->val << "(" << avg->pop << ")";
    avg->val = (avg->val * avg->pop + entry) / (avg->pop + 1);
    s << ", avg now is " << avg->val;

    log_->debugStream() << s.str();
    ++(avg->pop);
    avg = nullptr;
  }


} // namespace algol
