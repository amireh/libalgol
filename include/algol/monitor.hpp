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

#ifndef H_ALGOL_MONITOR_H
#define H_ALGOL_MONITOR_H

#include <map>

// dakapi
#include "algol/algol.hpp"
#include "algol/logger.hpp"

namespace algol {

  class monitor;


  class monitor : public logger {
  public:

    static monitor& singleton();

    typedef uint16_t stat_id;
    typedef string_t stat_key;
    typedef uint64_t stat_val;

    /** a cumulative average statistic */
    struct cavg_t {
      uint64_t  pop; // the cumulative average's population; nr of entries
      uint64_t  val; // the actual cumulative average value
    };

    typedef std::map<stat_id, uint64_t> stats_t;
    typedef std::map<stat_id, std::vector<cavg_t*> > avg_stats_t;

    explicit monitor();
    virtual ~monitor();
    monitor(const monitor&) = delete;
    monitor& operator=(const monitor&) = delete;

    /** Returns all the stats tracked by the monitor. */
    stats_t const& stats() const;
    avg_stats_t const& avg_stats() const;

    stat_val stat(stat_id);
    std::vector<cavg_t*> const& avg_stat(stat_id);

    /**
     * Registers the provided stat and generates the numerical UID that should
     * be used to modify it.
     *
     * If the given stat is already registered, a value of 0 will be returned.
     *
     * @note
     * Stat alphabetical AND numerical ids are unique.
     */
    stat_id track_stat(string_t const& name);

    stat_id track_avg_stat(string_t const& name);

    /** increments the identified stat by 1 */
    void inc_stat(stat_id stat);

    /** decrements the identified stat by 1 */
    void dec_stat(stat_id stat);

    /**
     * adds the given entry to the cumulative average stat's population
     * and recalculates the average
     */
    void avg_stat(stat_id stat, uint64_t entry);

    /**
     * Returns the unique name for the stat with the given UID if found,
     * and "InvalidStatIdentifier" if not.
     */
    string_t const& to_string(stat_id);

  private:
    typedef std::map<stat_id, string_t> stat_names_t;

    stats_t       stats_;
    avg_stats_t   avg_stats_;
    stat_names_t  stat_names_;

    static monitor* __instance;
    static uint32_t __guid;
  };

#define TRACK_STAT(subject, item, item_string) \
  monitor::stat_id subject::item = monitor::singleton().track_stat(item_string);
#define TRACK_AVG_STAT(subject, item, item_string) \
  monitor::stat_id subject::item = monitor::singleton().track_avg_stat(item_string);

#define INC_STAT(item) monitor::singleton().inc_stat(item);
#define DEC_STAT(item) monitor::singleton().dec_stat(item);
#define AVG_STAT(item, val) monitor::singleton().avg_stat(item, val);

} // namespace algol

#endif // H_ALGOL_MONITOR_H
