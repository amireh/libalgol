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

#ifndef H_ALGOL_CONFIGURABLE_H
#define H_ALGOL_CONFIGURABLE_H

#include "algol/algol.hpp"
#include <array>

namespace algol {

  class configurator;

  /**
   * \addtogroup Core
   * @{
   * @class configurable
   *
   * Configurables can register themselves with the dakapi server to be called
   * when the server configuration is parsed and processed.
   */
  class configurable {
    public:

    /**
     * Default ctor, does not subscribe to any context: you must manually do that by calling
     * configurable::subscribe_context()
     */
    explicit configurable();

    /**
     * Subscribes this instance to every given context
     *
     * @note
     * You clearly don't have to call configurator::subscribe_context() anymore
     * if you use this constructor.
     */
    explicit configurable(std::vector<string_t> contexts);

    configurable(const configurable&);
    configurable& operator=(const configurable&);
    virtual ~configurable();

    /** called whenever a cfg setting is encountered and parsed */
    virtual void set_option(string_t const& key, string_t const& value)=0;

    inline virtual void on_array_start() {}
    inline virtual void on_array_end() {}

    inline virtual void on_map_start() {}
    inline virtual void on_map_key(string_t const&) {}
    inline virtual void on_map_end() {}

    /**
     * Called when the config section is fully parsed, implementations
     * can do the actual configuration here if needed.
     *
     * @note
     * Sometimes this isn't really required as an implementation could configure
     * itself on-the-fly as set_option() is called, but others might have dependant
     * options.
     */
    inline virtual void configure() { }

    /** is this instance subscribed to the passed configuration context? */
    bool subscribed_to_context(string_t const&);

    protected:
    friend class configurator;

    /** this instance will be called whenever the given context is encountered */
    void subscribe_context(string_t const&);

    /**
     * when a configurable is subscribed to more than 1 context,
     * this string can be used to determine the current context being parsed
     */
    string_t current_ctx_;

    std::vector<string_t> cfg_contexts_;

    void copy(const configurable& src);
  };
  /** @} */
}

#endif
