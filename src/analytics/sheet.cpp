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

#include "algol/analytics/sheet.hpp"
#include "algol/analytics/tracker.hpp"
#include "algol/utility.hpp"

namespace algol {
namespace analytics {

  sheet_t::sheet_t(string_t const& title, sheet_t *parent)
  : parent_(parent),
    title_(title)
  {

    if (parent_) {
      parent_->__add_child(this);
    }

  }

  sheet_t::~sheet_t()
  {
    // std::cout << "destroying sheet_t '" << title_ << "'\n";
    parent_ = nullptr;

    for (auto t : trackers_)
      t->detach_sheet();

    while (!children_.empty())
    {
      delete children_.back();
      children_.pop_back();
    }

    trackers_.clear();
  }

  void sheet_t::add_stat(string_t const& key, int value)
  {
    num_stats_t::iterator finder = num_stats_.find(key);
    if (finder != num_stats_.end())
    {
      finder->second = value;
      return;
    }

    num_stats_.insert(std::make_pair(key, value));
  }

  void sheet_t::inc_stat(string_t const& key, int step)
  {
    num_stats_t::iterator finder = num_stats_.find(key);
    if (finder != num_stats_.end())
    {
      finder->second += step;
      return;
    }

    num_stats_.insert(std::make_pair(key, step));
  }

  void sheet_t::bool_stat(string_t const& key, bool value)
  {
    boolean_stats_t::iterator finder = boolean_stats_.find(key);
    if (finder != boolean_stats_.end())
    {
      finder->second = value;
      return;
    }

    boolean_stats_.insert(std::make_pair(key, value));
  }

  void sheet_t::add_stat(string_t const& key, string_t const& value)
  {
    literal_stats_t::iterator finder = literal_stats_.find(key);
    if (finder != literal_stats_.end())
    {
      finder->second = value;
      return;
    }

    literal_stats_.insert(std::make_pair(key, value));
  }

  sheet_t*  sheet_t::parent()
  {
    return parent_;
  }

  void sheet_t::set_parent(sheet_t* parent)
  {
    parent_ = parent;
  }

  bool sheet_t::has_parent() const
  {
    return parent_ != nullptr;
  }

  string_t const& sheet_t::title() const
  {
    return title_;
  }

  sheet_t::literal_stats_t const& sheet_t::literal_stats() const
  {
    return literal_stats_;
  }

  sheet_t::boolean_stats_t const& sheet_t::boolean_stats() const
  {
    return boolean_stats_;
  }
  sheet_t::num_stats_t const& sheet_t::numerical_stats() const
  {
    return num_stats_;
  }

  void sheet_t::__add_child(sheet_t* child)
  {
    children_.push_back(child);
  }

  sheet_t& sheet_t::operator[](string_t const& title)
  {
    for (auto child : children_)
      if (child->title() == title) return *child;

    sheet_t *child = new sheet_t(title, this);

    return *child;
  }

  sheet_t::children_t const& sheet_t::children() const
  {
    return children_;
  }

  static void handle_child(bson::BSONObjBuilder &p, sheet_t* sheet)
  {
    bson::BSONObjBuilder c;
    for (auto stat : sheet->numerical_stats())
    {
      c.append(stat.first, stat.second);
      // std::cout << sheet->title() << ":\t" << stat.first << " => " << stat.second << '\n';
    }
    for (auto stat : sheet->boolean_stats())
    {
      c.append(stat.first, stat.second);
      // std::cout << sheet->title() << ":\t" << stat.first << " => " << stat.second << '\n';
    }

    for (auto stat : sheet->literal_stats())
    {
      c.append(stat.first, stat.second);
      // std::cout << sheet->title() << ":\t" << stat.first << " => " << stat.second << '\n';
    }

    for (auto child : sheet->children())
      handle_child(c, child);

    p.append(sheet->title(), c.obj());
  }

  void sheet_t::commit() {

    if (is_empty()) {
      std::cout << "sheet is empty! i will not be committed:"
        << num_stats_.size()
        << boolean_stats_.size()
        << literal_stats_.size() << '\n';
      return;
    }

    bson::BSONObjBuilder p;
    //~ p.genOID();

    for (auto stat : numerical_stats()) {
      p.append(stat.first, stat.second);
    }
    for (auto stat : boolean_stats()) {
      p.append(stat.first, stat.second);
    }
    for (auto stat : literal_stats()) {
      p.append(stat.first, stat.second);
    }

    for (auto child : children())
    {
      handle_child(p, child);
    }

    string_t sheet_str(p.obj().jsonString());
    // std::cout << "JSON msg: \n" << sheet_str << '\n';

    message m(sheet_str);
    communicator().send(m, "analytics", algol_app().name);
  }

  void sheet_t::__add_tracker(tracker* t) {
    trackers_.push_back(t);
  }

  sheet_t::trackers_t sheet_t::trackers() {
    return trackers_;
  }

  bool sheet_t::is_empty() const {
    bool empty =
      numerical_stats().empty() &&
      literal_stats().empty() &&
      boolean_stats().empty();

    for (sheet_t *child : children())
      empty = empty && child->is_empty();

    return empty;
  }
}
}
