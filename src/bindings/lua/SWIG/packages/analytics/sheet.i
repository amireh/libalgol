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

%module lua_algol
%{
  #include <algol/analytics/sheet.hpp>
%}

#ifndef H_ALGOL_ANALYTICS_SHEET_T_H
#define H_ALGOL_ANALYTICS_SHEET_T_H

#include "algol/algol.hpp"
#include "algol/messaging/message.hpp"
#include "algol/messaging/communicator.hpp"

#include <list>
#include <map>

#ifndef ALGOL_NO_BSON
  #include <bson/bson.h>
#endif

#ifdef localtime
  #undef localtime
  #define localtime localtime
#endif

namespace algol {
namespace analytics {

  class tracker;
  /**
   * \addtogroup Analytics
   * @{
   * @class sheet_t
   * An analytics statistics sheet that's used by analytics trackers
   * to track information and submit them to the analytics platform.
   */
  class sheet_t {
  public:
    typedef std::map<string_t, uint32_t> num_stats_t;
    typedef std::map<string_t, string_t> literal_stats_t;
    typedef std::list<sheet_t *> children_t;
    typedef std::list<tracker *> trackers_t;

    sheet_t(string_t const& title, sheet_t *parent = nullptr);
    virtual ~sheet_t();

    /** Tracks a numerical stat. */
    void add_stat(string_t const& key, int value);

    /** Increments (and tracks, if needed) a stat. Useful for counters. */
    void inc_stat(string_t const& key, int step = 1);

    %rename(add_stat) bool_stat;
    void bool_stat(string_t const& key, bool value);

    /** Tracks a literal stat. */
    void add_stat(string_t const& key, string_t const& value);

    /**
     * Creates a nested, child sheet_t within the current one identified
     * by the given title.
     *
     * This is the preferred interface for creating sub-contexts, as it
     * makes for a natural hash-style access. For example:
     *   my_sheet_t["requests"]["errors"].inc_stat("Bad Header", 1);
     */
    sheet_t& operator[](string_t const& child_title);

    /**
     * Submits the sheet_t to the analytics platform.
     *
     * @remark
     * This is a blocking (synchronous) method.
     *
     * @note
     * sheet_ts are internally serialized as JSON when submitted.
     */
    void commit();

    /** The title of this sheet_t. */
    string_t const& title() const;

    /** The parent container sheet_t, if any. */
    sheet_t*  parent();

    /**
     * Assigns a parent for this sheet_t. Children sheet_ts are visible
     * only within their parent's scope.
     *
     * Nesting sheet_ts allows for creating a hierarchical analytics
     * sheet_t that are semantically useful in most contexts.
     *
     * @note
     * There is no need to manually assign parents; thanks to the
     * overloaded [] operator, sheet_ts can be nested via an easy
     * interface.
     */
    void set_parent(sheet_t*);

    /** Is this a nested sheet_t? */
    bool has_parent() const;

    /** All numerical stats tracked within this sheet_t (excluding children's). */
    num_stats_t  const& numerical_stats() const;

    /** All literal stats tracked within this sheet_t (excluding children's). */
    literal_stats_t  const& literal_stats() const;

    /** Direct accessor for first-class nested sheet_ts. */
    children_t const& children() const;

    /**
     * Assigns the given sheet_t as one of this sheet_t's children.
     *
     * @remark
     * This is managed internally by sheet_ts when set_parent() is called.
     */
    void __add_child(sheet_t*);
  };

  %extend sheet_t {
    sheet_t& child(string_t const& title) {
      return (*$self)[title];
    }
  };
  /** @} */
}
}

#endif
