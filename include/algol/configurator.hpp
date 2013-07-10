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

#ifndef H_ALGOL_CONFIGURATOR_H
#define H_ALGOL_CONFIGURATOR_H

#include "algol/algol.hpp"
#include "algol/logger.hpp"
#include "algol/configurable.hpp"

#include <map>

#include <yajl/yajl_parse.h>
#include <yajl/yajl_gen.h>

namespace algol {

  /**
   * \addtogroup Core
   * @{
   * @class configurator
   *
   * Parses a JSON configuration sheet and calls all subscribed configurable objects
   * with their options.
   */
  class configurator : public logger {
  public:

    configurator(string_t const& json_data);
    configurator(std::ifstream& json_file_handle);

    virtual ~configurator();
    configurator(const configurator&) = delete;
    configurator& operator=(const configurator&) = delete;

    static void init();
    static void silence();

    typedef struct {
      bool     valid;
      string_t status;
    } parser_rc;

    /**
     * performs the actual parsing and configuration of subscribed instances
     */
    void run();

    parser_rc validate(string_t const& json);

    /**
     * subscribed the given configurable to its configuration context;
     * once that context is encountered in a configuration file, the object
     * will be passed the options to handle them
     *
     * @note
     * a context can be assigned at most 1 configurable at any time,
     * but a configurable can handle many contexts
     */
    static void subscribe(configurable*, string_t const& context);
    static void unsubscribe(configurable*, string_t const& context);

    int __on_json_map_start();
    int __on_json_map_key(const unsigned char*, size_t);
    int __on_json_map_val(const unsigned char*, size_t);
    int __on_json_map_end();
    int __on_json_array_start();
    int __on_json_array_end();

  private:

    /**
     * Is the JSON key a keyword reserved by the configurator?
     *
     * Currently, only "include" is reserved.
     * */
    bool is_reserved(const string_t& ctx);

    void parse_string(string_t const& data);

    typedef std::list<configurable*> configurables_t;
    typedef std::map<string_t, configurables_t> subs_t;
    static subs_t subs_;
    static bool init_;
    static bool silent_;

    string_t data_;

    string_t        curr_key_;
    string_t        curr_val_;
    string_t        curr_ctx_;
    int             depth_;
    configurables_t *curr_subs_;

    enum {
      yajl_continue = 1, /** continue parsing */
      yajl_stop = 0 /** stop parsing */
    };

  };
  /** @} */
} // namespace algol

#endif // H_ALGOL_LOG_MANAGER_H
