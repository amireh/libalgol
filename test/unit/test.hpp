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

#ifndef H_ALGOL_TEST_H
#define H_ALGOL_TEST_H

#include "algol/logger.hpp"

#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

namespace algol {

  typedef int result_t;

  class test : public logger {
  public:

    enum {
      passed,
      failed
    };

    explicit test(std::string in_name, bool autorun_io = true)
    : io_service_(),
      name_(in_name),
      result_(failed),
      logger(string_t(in_name + "_test").c_str())
    {

      log_->infoStream() << "starting";
      if (autorun_io)
        io_service_.run();
    }

    virtual ~test()
    {
      stop();
    }

    /**
     * Override with the actual test implementation.
     */
    virtual result_t run(int argc, char** argv)=0;
    virtual void stop()
    {
      // if (!io_service_.stopped()) {
        std::cout << "-+-+ test: " << name_ << " shutting down...\n";
        io_service_.stop();
      // }
    }

    boost::asio::io_service const& get_io_service() const
    {
      return io_service_;
    }

    /** Override this if you need any functionality to be called within the program's main() */
    inline virtual void main(int, char **) {

    };

    inline virtual void report(result_t rc) {
      log_->infoStream() << "finished: " << status_to_string(rc);
    }

  protected:

    /**
     * Marks the beginning of a new stage in the test identified by @name.
     *
     * If you pass @timed=true, the stage will be stopwatched and the time
     * elapsed (in ms) inside it will be displayed when the test is over.
     *
     * The result of the stage is determined by the results of all assertions
     * occuring inside it.
     */
    void stage(string_t, bool) {

    }

    /**
     * Asserts the given condition and registers it for display in the
     * result dump as @state.
     *
     * If the condition is false, the test will NOT abort.
     */
    void soft_assert(string_t, bool) {
      // TODO: track state
    }

    /**
     * Asserts the given condition and registers it for display in the
     * result dump as @state.
     *
     * If the condition is false, the test WILL abort.
     */
    void hard_assert(string_t, bool condition) {
      assert(condition);
    }

    boost::asio::io_service io_service_;

    std::string name_;
    result_t result_;

    inline string_t status_to_string(result_t rc) {
      if (rc == passed) {
        return "\033[0;32mpassed\033[0m";
      } else {
        return "\033[0;31mfailed\033[0m";
      }
    }
  private:
    test(const test&) = delete;
    test& operator=(const test&) = delete;

    struct stage_t {
      string_t        name;
      bool            timed;
      unsigned long   elapsed_ms;
      result_t        result;
    };
  };
}
#endif
