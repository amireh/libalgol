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

#ifndef H_ALGOL_REGEX_PCRE_H
#define H_ALGOL_REGEX_PCRE_H

#include "algol/algol.hpp"

#include <pcrecpp.h>

namespace algol {

  /**
   * \addtogroup Core
   * @{
   * @class regex
   * An implementation of a PCRE (Perl) regular expression object that can be used
   * to match a pattern against given strings.
	 */
	class regex
	{
	public:

    regex();

    /**
     * Creates and compiles a regular expression as per the given PERL regex
     * pattern.
     *
     * @warning
     * If the pattern is invalid and the expression could not be compiled,
     * an exception of type algol::invalid_regex_pattern will be thrown.
     */
		regex(std::string const& pattern);
		~regex();

		regex& operator=(const regex& rhs);
		regex(const regex& src);
    bool operator==(string_t const& pattern);

    /**
     * Returns true if the given input is captured by this regular expression
     * pattern.
     *
     * @warning
     * An expression of type algol::invalid_regex will be thrown if match()
     * is called without a successfully compiled regex object.
     */
    bool match(string_t const& input);

    /**
     * Compiles the regular expression as per the given pattern.
     *
     * If the pattern is invalid and the expression could not be compiled,
     * an exception of type algol::invalid_regex_pattern will be thrown.
     *
     * @note
     * This method does not have to be called if you've constructed the regex
     * object using the constructor that takes a pattern, it will be compiled
     * automatically for you.
     */
    void _compile(string_t const& pattern);

    /** has this regex been successfully compiled and is ready for use? */
    bool is_compiled() const;

    /** the pattern that was used to construct this regex */
    string_t const& pattern() const;

    /**
     * Destroys the existing (if any) underlying regex object and reconstructs
     * it. This is useful when you've changed the options and want to apply
     * them to existing regex objects.
     */
    void _recompile();

    /**
     * use this object to configure PCRE, these options will be used for all
     * compiled regex objects
     */
    static pcrecpp::RE_Options& options();

    /** gives access to the underlying PCRE regex object */
    pcrecpp::RE* _obj();

		protected:

		pcrecpp::RE* re_;
    static pcrecpp::RE_Options opts_;

    void copy(const regex& src);
	}; // end of regex class

  /** @} */

} // end of namespace algol

#endif
