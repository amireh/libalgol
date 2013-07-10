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

#include "algol/file_manager.hpp"
#include "algol/utility.hpp"

#include <map>

namespace algol {

  file_manager* file_manager::__instance = 0;
  uint64_t file_manager::download_id_ = 0;

  file_manager::file_manager()
  : configurable({"file manager"}),
    logger("file manager")
  {
    config_.dist_from_root = "1";
  }

  file_manager::~file_manager()
  {
    br_cleanup();
  }

  file_manager& file_manager::singleton() {
    if (!__instance)
      __instance = new file_manager();

    return *__instance;
  }

  void file_manager::resolve_paths()
  {
    using boost::filesystem::create_directory;
    using boost::filesystem::is_directory;

    int dist_from_root;
    try
    {
      dist_from_root = utility::convertTo<int>(config_.dist_from_root);
    } catch (algol::bad_conversion& e)
    {
      std::cerr << "algol: file_manager: invalid 'dist_from_root' option, defaulting to 1\n";
      dist_from_root = 1;
    }

    // locate the binary and build its path
    // use binreloc and boost::filesystem to build up our paths
    int brres = br_init(0);
    if (brres == 0) {
      std::cerr << "algol: file_manager: binreloc could not be initialised, can not resolve paths\n";
    }
    char *p = br_find_exe_dir(".");
    bin_path_ = std::string(p);
    free(p);
    bin_path_ = path_t(bin_path_).make_preferred();

    // application root path:
    path_t root = path_t(bin_path_);
    for (int i=0; i < dist_from_root; ++i) {
      root = root.remove_leaf();
    }

    root_path_ = root.make_preferred();

    // create the log directory if needed
    //log_path_ = (path(root_path_) / path(config_.log_dir)).make_preferred();
    //if (!is_directory(path(log_path_).root_directory()))
    //  create_directory(path(log_path_).root_directory());

    // std::cout << "Binary path: " <<  bin_path_ << "\n";
    // std::cout << "Root path: " <<  root_path_ << "\n";
    // std::cout << "Log path: " <<  log_path_ << "\n";
  }

  void file_manager::set_option(string_t const& key, string_t const& value)
  {
    if (key == "dist_from_root" || key == "dist from root") {
      config_.dist_from_root = value;
    }
    else {
      std::cerr << "unknown file_manager config setting '" << key << "' => '" << value << "', discarding";
    }
  }

  path_t const& file_manager::root_path() const
  {
    return root_path_;
  }

  path_t const& file_manager::bin_path() const
  {
    return bin_path_;
  }

  static size_t on_curl_data(char *buffer, size_t size, size_t nmemb, void *userdata)
  {
    return file_manager::singleton().__on_curl_data(buffer, size, nmemb, userdata);
  }

  bool file_manager::load_file(std::ifstream &fs, string_t& out_buf)
  {
    if (!fs.is_open() || !fs.good()) return false;

    while (fs.good())
      out_buf.push_back(fs.get());

    out_buf.erase(out_buf.size()-1,1);

    //~ log_->debugStream() << "read file into memory: \n" << out_buf;
    return true;
  }

  bool file_manager::get_resource(string_t const& resource_path, string_t& out_buf)
  {
    bool is_remote = (resource_path.substr(0,4) == "http");
    if (is_remote)
      return get_remote(resource_path, out_buf);

    using boost::filesystem::exists;

    path_t p(resource_path);

    if (!exists(p))
    {
      p = root_path_ / p;
      if (!exists(p)) {
        log_->errorStream() << "local resource does not exist: " << resource_path;
        return false;
      }
    }

    std::ifstream fs(p.string());
    // open the file and load it into the string
    if (!fs.is_open() || !fs.good())
    {
      log_->errorStream() << "local resource is not readable; " << resource_path;
      return false;
    }

    bool result = load_file(fs, out_buf);
    fs.close();

    return result;
  }

  bool file_manager::get_remote(string_t const& in_URL, string_t& out_buf)
  {

    // now use CURL to hit dakapi
    CURL* curl_ = curl_easy_init();
    CURLcode curlrc_;

    if (!curl_) {
      log_->errorStream() << "unable to resolve URL " << in_URL << ", aborting remote download request";
      return false;
    }

    download_t *dl = new download_t();
    dl->id = ++download_id_;
    dl->buf = &out_buf;
    dl->status = false;
    dl->uri = in_URL;

    char curlerr[CURL_ERROR_SIZE];

    curl_easy_setopt(curl_, CURLOPT_ERRORBUFFER, curlerr);
    curl_easy_setopt(curl_, CURLOPT_URL, in_URL.c_str());
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, &on_curl_data);
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, dl);

    curlrc_ = curl_easy_perform(curl_);
    if (curlrc_ != 0) {
      log_->errorStream() << "a CURL error was encountered; " << curlrc_ << " => " << curlerr;
      delete dl;

      curl_easy_cleanup(curl_);

      return false;
    }

    long http_rc = 0;
    curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &http_rc);

    if (http_rc != 200) {
      log_->errorStream() << "remote server error, HTTP code: " << http_rc << ", download failed";
      delete dl;

      curl_easy_cleanup(curl_);

      return false;
    }

    /* always cleanup */
    curl_easy_cleanup(curl_);
    delete dl;
    return true;
  }

  size_t file_manager::__on_curl_data(char *buffer, size_t size, size_t nmemb, void *userdata)
  {
    download_t *dl = (download_t*)userdata;

    size_t realsize = size * nmemb;
    (*dl->buf) += string_t(buffer);

    return realsize;
  }

  file_manager::file_listing_t
  file_manager::get_directory_listing(string_t const& directory, string_t const& ext)
  {
    using namespace boost::filesystem;

    path_t p(directory);
    file_listing_t files;

    if (!is_directory(p))
      return files;

    directory_iterator end_it;
    for (directory_iterator it(p); it != end_it; ++it)
    {
      if (!ext.empty() && it->path().extension() != ext) {
        log_->debugStream() << "skipping " << it->path() << " as its extension does not match " << ext;
        continue;
      }

      files.push_back(it->path().string());
      log_->debugStream() << "file found: " << it->path();
    }

    return files;
  }

  bool file_manager::exists(string_t const& path) {
    return boost::filesystem::exists(path_t(path));
  }

} // namespace algol
