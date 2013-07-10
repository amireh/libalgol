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

#include <algol/identifiable.hpp>

namespace algol {

  identifiable::~identifiable()
  {
  }

  identifiable::identifiable()
  : uuid_("0")
  {
  }
  identifiable::identifiable(uuid_t uuid)
  : uuid_(uuid)
  {
  }

  identifiable::identifiable(identifiable* parent)
  : uuid_("0")
  {
    parent->__add_child(this);
  }

  identifiable::identifiable(const identifiable& src)// : Caster(this)
  {
    clone(src);
  }

  identifiable& identifiable::operator=(const identifiable& rhs)
  {
    if (this != &rhs) // check for self-assignment
      clone(rhs);

    return (*this);
  }

  void identifiable::clone(const identifiable& src)
  {
    uuid_ = src.uuid_;
  }

  void identifiable::__set_uuid(uuid_t uuid)
  {
    if (uuid_ != "0")
    {
      string_t msg =
        "Attempting to re-assign an object's UUID after it's been set! Object: "
        + uuid_;
      throw integrity_violation(msg);
    }

    uuid_ = uuid;

    on_uuid_set();

    for (identifiable* child : children_)
      child->__set_uuid(get_uuid());
  }

  uuid_t identifiable::get_uuid() const {
    return uuid_;
  }

  bool identifiable::operator==(const identifiable& rhs)
  {
    return rhs.uuid_ == this->uuid_;
  }

  void identifiable::__add_child(identifiable* child) {
    children_.push_back(child);
  }

  void identifiable::on_uuid_set() {
  }
}
