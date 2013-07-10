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

#ifndef H_ALGOL_SHELL_DELEGATE_H
#define H_ALGOL_SHELL_DELEGATE_H

#include <algol/algol.hpp>
#include <algol/admin/dispatcher.hpp>

namespace algol {
namespace admin {
namespace shell {

  /**
   * \addtogroup Console
   * @{
   * @class delegate
   *
   * Delegates represent target applications that algolSH can connect to,
   * they handle the input feed and interpret command responses from
   * the target's bot.
   */
  class delegate {
  public:

    /**
     * Default constructor.
     *
     * @param target The target application this delegate represents.
     */
    inline delegate(const string_t &target) : target_(target) { }
    inline virtual ~delegate() { }

    /**
     * Delegates are responsible for handling incoming commands and interpreting them.
     * In this method, messages should be bound to handlers.
     */
    virtual void bind( admin::dispatcher& );

    /**
     * Commands fed by the user can be evaluated before being sent off, and here is
     * the place to intercept them. For example, a command might require certain arguments,
     * you can validate the input feed in this method, and reject or send the command accordingly.
     *
     * @param feed The input fed by the user to algolSH.
     * @param tokens The input feed broken off into tokens split by spaces (' ')
     *
     * The default behaviour is to send the first token as the command. IE, for a feed of "configure foobar",
     * the command sent will be "foobar", and the rest of the feed will be discarded;
     */
    virtual void on_feed( const string_t &feed, const std::vector<string_t> &tokens );

    /**
     * If you need to override the default behaviour for handling incoming responses
     * for sent commands then override this method.
     *
     * The default behaviour is:
     *  1. when the feedback is set to error and an "Error" property exists, print msg["Error"] and [ FAILED ] in red
     *  2. if the message has the no_format option, print msg["Data"]
     *  3. if the message has a "Status" property, display it and append an [ OK ] in green
     *  4. print an "Unknown format" error along with a dump of the message (using message::dump())
     */
    virtual void on_response(const message&);

    /** Return the target application this delegate represents */
    inline string_t const& target() const { return target_; }

  protected:
    virtual void send(message&);

  protected:
    string_t target_;
  };

  /** @} */
}
}
}

#endif
