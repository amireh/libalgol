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

#include "algol/configurable.hpp"
#include "algol/configurator.hpp"

namespace algol {

  configurable::configurable()
  {
  }

  configurable::configurable(std::vector<string_t> contexts)
  {
    for (auto ctx : contexts)
      subscribe_context(ctx);
  }

  configurable& configurable::operator=(const configurable& rhs)
  {
    if (this != &rhs) copy(rhs);

    return *this;
  }

  configurable::configurable(const configurable& src)
  {
    copy(src);
  }

  configurable::~configurable()
  {
    for (auto ctx : cfg_contexts_) {
      configurator::unsubscribe(this, ctx);
    }
    cfg_contexts_.clear();
  }

  void configurable::copy(const configurable& src)
  {
    current_ctx_ = src.current_ctx_;
    cfg_contexts_ = src.cfg_contexts_;
  }

  void configurable::subscribe_context(string_t const& ctx)
  {
    configurator::subscribe(this, ctx);
    cfg_contexts_.push_back(ctx);
  }

  bool configurable::subscribed_to_context(string_t const& in_ctx)
  {
    for (auto ctx : cfg_contexts_)
      if (ctx == in_ctx) return true;

    return false;
  }

}
