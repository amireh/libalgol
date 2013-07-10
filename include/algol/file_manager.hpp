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

#ifndef H_ALGOL_FILE_MANAGER_H
#define H_ALGOL_FILE_MANAGER_H

#include "algol/algol.hpp"
#include "algol/logger.hpp"
#include "algol/configurable.hpp"
#include "algol/binreloc/binreloc.h"

#include <sstream>
#include <fstream>
#include <boost/filesystem.hpp>
#include <curl/curl.h>

namespace algol {

  typedef boost::filesystem::path path_t;

  /**
   * \addtogroup Core
   * @{
   * @class file_manager
   *
   * Manages a logging system through log4cpp that can be used by an application
   * to log messages to a number of output devices; stdout, a file, or syslog.
   *
   * Logging devices and the format of messages are configurable.
   *
   * Actual logging of the messages should be done by by instances derived from
   * algol::logger (see algol/algol_logger.hpp).
   */
  class file_manager : public configurable, public logger {
  public:

    typedef std::list<string_t> file_listing_t;

    /* the file manager's config context is "file manager" */
    typedef struct {

      /**
       * dist_from_root:
       *  indicates how many directories to traverse up from the binary to reach
       *  the application's root directory
       *
       * default: "1" (application binary resides in /path/to/app/bin)
       *
       * alias keys: "dist from root"
       */
      string_t dist_from_root;
    } config_t;

    static file_manager& singleton();

    virtual ~file_manager();
    file_manager(const file_manager&) = delete;
    file_manager& operator=(const file_manager&) = delete;

    /**
     * Finds the fully-qualified paths of the application's binary, and its root.
     **/
    void resolve_paths();

    path_t const& root_path() const;
    path_t const& bin_path() const;

    typedef std::function<void (bool, const string_t&, string_t)> dl_cb_t;

    /**
     * a resource can be a remote file identified by a URL or a local file
     *  1. remote files: http://path/to/file
     *  2. local files: file://path/to/file
     */
    bool get_resource(string_t const& resource_path, string_t& out_buf);

    /**
     * downloads the file found at URL and stores it in out_buf
     *
     * @return true if the file was correctly DLed, false otherwise
     */
    bool get_remote(string_t const& URL, string_t& out_buf);

    /** same as above but outputs to file instead of buffer */
    bool get_remote(string_t const& URL, std::ofstream& out_file);

    /**
     * note: the stream must be open and this method will NOT close it
     */
    bool load_file(std::ifstream &file, string_t& out);

    bool exists(string_t const& path);

    /**
     * if ext is empty, all files in the given directory will be returned
     **/
    file_listing_t get_directory_listing(string_t const& directory, string_t const& ext);

    /** overridden from algol::configurable */
    virtual void set_option(string_t const& key, string_t const& value);

    size_t __on_curl_data(char *buffer, size_t size, size_t nmemb, void *userdata);
  private:
    explicit file_manager();
    static file_manager* __instance;

    config_t config_;
    path_t   root_path_,
             bin_path_;

    struct download_t {
      uint64_t        id;
      string_t        *buf;
      string_t        uri;
      dl_cb_t         callback;
      bool            status;
    };

    static uint64_t download_id_;
  };
  /** @} */

} // namespace algol

#endif // H_ALGOL_LOG_MANAGER_H
