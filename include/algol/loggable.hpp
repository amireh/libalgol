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

#ifndef H_ALGOL_LOGGABLE_H
#define H_ALGOL_LOGGABLE_H

namespace algol
{
	/**
   * \addtogroup Core
   * @{
   * @class loggable
   * Loggable objects respond to the operator<< for logging to an std::ostream.
	 */
	class loggable
	{

	public:
		loggable() {}
		loggable& operator=(const loggable& rhs) { return *this; }
		loggable(const loggable& src) {}
		virtual ~loggable() {}

    inline friend std::ostream& operator<<(std::ostream& in_stream, loggable* in_loggable)
    {
      return in_loggable->to_stream(in_stream);
    }

    inline friend std::ostream& operator<<(std::ostream& in_stream, loggable& in_loggable)
    {
      return in_loggable.to_stream(in_stream);
    }

		protected:

    virtual std::ostream& to_stream(std::ostream&)=0;

	}; // end of loggable class

  /** @} */
} // end of algol namespace
#endif
