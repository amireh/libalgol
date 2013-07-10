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

#include "algol/regex.hpp"

namespace algol {

  pcrecpp::RE_Options regex::opts_;
  pcrecpp::RE_Options& regex::options()
  {
    return opts_;
  }

  regex::regex()
  : re_(nullptr)
  {
  }

  regex::regex(string_t const& pattern)
  : re_(nullptr)
  {
    _compile(pattern);
  }

  regex& regex::operator=(const regex& rhs)
  {
    if (this != &rhs) copy(rhs);

    return *this;
  }

  regex::regex(const regex& src)
  {
    copy(src);
  }

  regex::~regex()
  {
    if (re_ != nullptr) {
      delete re_;
    }
    re_ = nullptr;
  }

  bool regex::operator==(const string_t& pattern)
  {
    return (re_->pattern() == pattern);
  }

  void regex::copy(const regex& src)
  {
    re_ = src.re_;
  }

  string_t const& regex::pattern() const
  {
    return re_->pattern();
  }

  pcrecpp::RE* regex::_obj()
  {
    return re_;
  }

  void regex::_compile(string_t const& pattern)
  {
    re_ = new pcrecpp::RE(pattern, opts_);

    if (!re_->error().empty())
    {
      string_t msg = "PCRE compilation failed for pattern '" + pattern + "', error: " + re_->error();
      delete re_;
      re_ = nullptr;
      throw invalid_regex_pattern(msg);
    }

  }

  void regex::_recompile()
  {
    if (!is_compiled()) {
      throw illegal_operation("attempt to re-compiled a non-compiled regex object!");
    }

    string_t lpattern = pattern();

    delete re_;
    re_ = nullptr;

    _compile(lpattern);
  }

  bool regex::is_compiled() const
  {
    return re_ != nullptr;
  }

  bool regex::match(string_t const& input)
  {
    return re_->FullMatch(input);
  }
}
