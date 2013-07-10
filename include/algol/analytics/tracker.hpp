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

#ifndef H_ALGOL_ANALYTICS_TRACKER_H
#define H_ALGOL_ANALYTICS_TRACKER_H

#include "algol/analytics/sheet.hpp"

namespace algol {
namespace analytics {

  /**
   * \addtogroup Analytics
   * @{
   * @class tracker
   *
   * Trackers can build analytics sheets and share them between each
   * other to track stats collectively and submit them when they've
   * gathered enough data (or, their job is done!).
   *
   * The intended usage scenario is for one component (who's a tracker)
   * build an analytics sheet, then attach it to sub/peer-components.
   * All the stats set by those trackers will be kept in one sheet and
   * then committed in an orderly fashion.
   */
  class tracker {
  public:

    /** Default ctor, no sheet will be built or attached. */
    tracker();

    /** Convenience constructor for building a sheet when initialized. */
    tracker(string_t const& title);

    /** Trackers are copiable; the sheet will be be simply linked. */
    tracker& operator=(const tracker& rhs);
    tracker(const tracker& src);

    /**
     * If a sheet is attached, it will be destroyed when the tracker
     * is destroyed.
     */
    virtual ~tracker();

    /**
     * A handy accessor for the sheet this tracker is tracking. See
     * analytics::sheet_t to learn how to use them.
     */
    sheet_t& sheet();

    /** Whether a sheet has been built or attached. */
    bool tracking() const;

    /**
     * Assigns the given sheet as this tracker's main sheet. This
     * is useful for passing a sheet around between components when
     * it makes sense to track various stats in the same sheet instead
     * of having each tracker build (and commit) its own.
     */
    void attach_sheet(sheet_t *sheet);

  protected:

    /**
     * Prepares a brand new sheet unless a sheet has already been
     * built or attached, in which case an exception of type
     * analytics::out_of_ink will be raised.
     *
     * The reason for not allowing sheets to be re-built or destroyed
     * is that they're meant to be shared between components, and there's
     * simply no elegant way to guarantee that other trackers tracking
     * this sheet or any of its children will behave themselves and not
     * cause a segfault.
     */
    void build_sheet(string_t const& title);

    sheet_t* ss_;
  private:
    friend class sheet_t;

    void detach_sheet();

  }; // end of tracker class

  /** @} */

} // end of namespace analytics
} // end of namespace algol

#endif
