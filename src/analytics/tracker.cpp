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

#include "algol/analytics/tracker.hpp"

namespace algol {
namespace analytics {

  tracker::tracker()
  : ss_(nullptr)
  {
  }

  tracker::tracker(string_t const& title)
  : ss_(nullptr)
  {
    build_sheet(title);
  }

  tracker& tracker::operator=(const tracker& rhs)
  {
    if (this != &rhs) ss_ = rhs.ss_;

    return *this;
  }

  tracker::tracker(const tracker& src)
  {
    ss_ = src.ss_;
  }

  tracker::~tracker()
  {
    // only the "master" tracker (the one who has the top-most sheet)
    // is responsible for destroying the sheet and its children
    if (ss_ && !ss_->has_parent())
      delete ss_; // it will take care of deleting children and unlinking their trackers

    ss_ = nullptr;
  }

  void tracker::attach_sheet(sheet_t *sheet)
  {
    if (tracking())
      throw out_of_ink();

    ss_ = sheet;
    ss_->__add_tracker(this);
  }

  sheet_t& tracker::sheet()
  {
    return *ss_;
  }

  void tracker::build_sheet(string_t const& title) {
    if (tracking())
      throw out_of_ink();

    ss_ = new sheet_t(title);
    ss_->__add_tracker(this);
  }

  bool tracker::tracking() const {
    return ss_ != nullptr;
  }

  void tracker::detach_sheet() {
    ss_ = nullptr;
  }

}
} // end of namespace algol
