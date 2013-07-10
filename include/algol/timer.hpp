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

#ifndef H_ALGOL_TIMER_H
#define H_ALGOL_TIMER_H

#include <wctype.h> /* tolower */
#include <ctime>
#include <sys/time.h>

namespace algol {

  /**
   * @struct timer_t
   * A simple stopwatch to calculate elapsed time of operations in millseconds.
   *
   * Usage:
   *  timer_t timer;
   *  timer.start();
   *  // do some work
   *  timer.stop();
   *  std::cout << timer.elapsed; // will print out in milliseconds the elapsed time
   */
  struct timer_t {
    inline timer_t()
    : seconds(0),
      useconds(0),
      elapsed(0)
    {

    }

    long seconds, useconds;

    unsigned long elapsed;

    inline void start() {
      gettimeofday(&__start, NULL);
    }

    inline void stop() {
      gettimeofday(&__end, NULL);
      seconds   = __end.tv_sec - __start.tv_sec;
      useconds  = __end.tv_usec - __start.tv_usec;
      elapsed   = ((seconds) * 1000 + useconds / 1000.0) + 0.5;
    }

    struct timeval __start, __end;

  };

}

#endif
