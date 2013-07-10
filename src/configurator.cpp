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

#include "algol/configurator.hpp"
#include "algol/file_manager.hpp"

namespace algol {

  configurator::subs_t configurator::subs_;
  bool configurator::init_ = false;
  bool configurator::silent_ = false;

  //void configurator::subscribe(configurable *cfg)
  //{
  //  subscribe(cfg, cfg->context());
  //}

  void configurator::silence() {
    silent_ = true;
  }

  void configurator::init()
  {
    subs_.clear();
    init_ = true;
  }

  void configurator::subscribe(configurable *cfg, const string_t& ctx)
  {
    if (!init_)
      init();

    if (subs_.find(ctx) == subs_.end()) {
      //~ ALGOL_LOG->warnStream() << "configuration context '" << ctx << "' is already registered";
      //~ std::cout << "configuration context '" << ctx << "' is already registered\n";
      subs_.insert(std::make_pair(ctx, configurables_t({})));
    }

    subs_.find(ctx)->second.push_back(cfg);
    if (!silent_)
      ALGOL_LOG->infoStream() << "subscribed a configurable service '" << ctx << "', there are now " << subs_.find(ctx)->second.size() << " subscribers to this context";
    //~ std::cout << "subscribed a configurable service '" << ctx << "'\n";
  }

  void configurator::unsubscribe(configurable* cfg, string_t const& ctx) {
    if (subs_.find(ctx) == subs_.end()) {
      ALGOL_LOG->errorStream() << "unsubscribing from a non-existent context '" << ctx << '\'';
      return;
    }

    subs_.find(ctx)->second.remove(cfg);
  }

  // the callbacks to handle the config parsing
  static yajl_callbacks vld_callbacks = {
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
  };

  static yajl_callbacks cfg_callbacks = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,

    // on a JSON string value
    [](void* ctx, const unsigned char* key, size_t len) -> int {
      configurator* cfg = (configurator*)ctx;
      return cfg->__on_json_map_val(key, len);
    },

    // on JSON map start
    [](void* ctx) -> int {
      configurator* cfg = (configurator*)ctx;
      return cfg->__on_json_map_start();
    },

    // on JSON map key
    [](void* ctx, const unsigned char* key, size_t len) -> int {
      configurator* cfg = (configurator*)ctx;
      return cfg->__on_json_map_key(key, len);
    },
    // on JSON map end
    [](void* ctx) -> int {
      configurator* cfg = (configurator*)ctx;
      return cfg->__on_json_map_end();
    },

    // on JSON array start,
    [](void* ctx) -> int {
      configurator* cfg = (configurator*)ctx;
      return cfg->__on_json_array_start();
    },

    // on JSON array end
    [](void* ctx) -> int {
      configurator* cfg = (configurator*)ctx;
      return cfg->__on_json_array_end();
    },
  };

  configurator::configurator(string_t const& data)
  : logger("configurator"),
    data_(data),
    depth_(0),
    curr_subs_(nullptr)
  {
  }

  configurator::configurator(std::ifstream& fh)
  : logger("configurator"),
    data_(),
    depth_(0),
    curr_subs_(nullptr)
  {
    // TODO: read file contents into string
    file_manager::singleton().load_file(fh, data_);
  }

  configurator::~configurator()
  {
    data_.clear();
  }

  configurator::parser_rc configurator::validate(string_t const& json) {
    parser_rc rc;

    yajl_status stat;
    yajl_handle hnd(yajl_alloc(&vld_callbacks, NULL, this));
    yajl_config(hnd, yajl_allow_comments, 1);

    stat = yajl_parse(hnd, (const unsigned char*)json.c_str(), json.size());

    if (stat != yajl_status_ok) {

      unsigned char *yajl_error = yajl_get_error(hnd, 1, (const unsigned char*)json.c_str(), json.size());
      string_t yajl_error_str((const char*)yajl_error);
      log_->errorStream()
        << "error parsing JSON config, bailing out #{"
        << stat << "} => " << yajl_error;
      yajl_free_error(hnd, yajl_error);
      yajl_free(hnd);

      rc.valid = false;
      rc.status = yajl_error_str;
      return rc;
    }

    yajl_free(hnd);

    rc.valid = true;
    return rc;
  }

  void configurator::run()
  {
    //~ log_->infoStream() << "configuring subscribers from JSON sheet";
    //~ log_->debugStream() << "JSON data: \n" << data_;
    parser_rc validation_rc = validate(data_);
    if (!validation_rc.valid) {
      log_->errorStream() << "Skipping configuration data because it's invalid. Error: '" << validation_rc.status << "'";
      return;
    }

    yajl_status stat;
    yajl_handle hnd(yajl_alloc(&cfg_callbacks, NULL, this));
    yajl_config(hnd, yajl_allow_comments, 1);

    stat = yajl_parse(hnd, (const unsigned char*)data_.c_str(), data_.size());

    if (stat != yajl_status_ok) {

      unsigned char *yajl_error = yajl_get_error(hnd, 1, (const unsigned char*)data_.c_str(), data_.size());
      log_->errorStream()
        << "error parsing JSON config, bailing out #{"
        << stat << "} => " << yajl_error;
      yajl_free_error(hnd, yajl_error);
      yajl_free(hnd);
      return;
    }

    yajl_free(hnd);

    //~ log_->infoStream() << "configuration was successful";
  }

  void configurator::parse_string(const string_t &data)
  {
    configurator cfg(data);
    cfg.run();
  }


  int configurator::__on_json_map_start()
  {
    ++depth_;

    if (curr_subs_)
      for (auto sub : (*curr_subs_))
        sub->on_map_start();

    return yajl_continue;
  }

  int configurator::__on_json_map_key(const unsigned char* key, size_t len)
  {
    curr_key_.clear();
    curr_key_ = std::string(reinterpret_cast<const char*>(key), len);

    if (is_reserved(curr_key_))
      return yajl_continue;

    if (!curr_subs_) {
      subs_t::iterator finder = subs_.find(curr_key_);
      curr_ctx_ = curr_key_;

      if (finder != subs_.end())
      {
        curr_subs_ = &finder->second;
        for (auto sub : *curr_subs_)
          sub->current_ctx_ = curr_ctx_;

        //~ log_->infoStream() << "configuring '" << curr_key_ << "'";
      } else {
        log_->warnStream() << "no subscribed configurable(s) for context '" << curr_key_ << "', skipping config";
      }
    }

    if (curr_subs_)
      for (auto sub : *curr_subs_)
        sub->on_map_key(curr_key_);

    return yajl_continue;
  }

  int configurator::__on_json_map_val(const unsigned char *val, size_t len)
  {
    if (curr_key_ == "include")
    {
      curr_val_.clear();
      curr_val_ = string_t(reinterpret_cast<const char*>(val), len);

      log_->infoStream() << "including external config file: " << curr_val_;

      string_t data;
      if (file_manager::singleton().get_resource(curr_val_, data)) {
        parse_string(data);
      }

      return yajl_continue;
    }

    else if (curr_key_ == "include all")
    {
      curr_val_.clear();
      curr_val_ = string_t(reinterpret_cast<const char*>(val), len);

      log_->infoStream() << "including all files in directory: " << curr_val_;

      string_t dir_path = curr_val_;
      string_t ext = "";
      // if the path contains an extension wildmark, we retrieve
      // files only of the matching extension, ie: /path/to/directory/*.js => *.js
      do {
        size_t wildmark_cur = dir_path.find('*');
        if (wildmark_cur != string_t::npos)
        {
          // make sure there's actually an extension after the asterisk
          if (wildmark_cur+1 >= dir_path.size()) {
            log_->errorStream() << "invalid wildmark in directory path, missing extension maybe? path: " << dir_path;
            break;
          }

          ext = curr_val_.substr(wildmark_cur + 1, curr_val_.size());
          log_->noticeStream() << "extension specified: " << ext;

          dir_path = dir_path.substr(0, wildmark_cur);
        }
      } while(false);

      file_manager::file_listing_t files(file_manager::singleton().get_directory_listing(dir_path, ext));
      // process each file
      for (auto file_path : files)
      {
        string_t data;
        if (file_manager::singleton().get_resource(file_path, data)) {
          parse_string(data);
        }
      }

      return yajl_continue;
    }

    curr_val_.clear();
    curr_val_ = string_t(reinterpret_cast<const char*>(val), len);

    if (curr_val_.find("%include%") == 0) {
      string_t resource_path = curr_val_.substr(9, curr_val_.size());
      string_t resource_data = "";

      log_->infoStream() << "Including the content of '" << resource_path << '\'';

      if (!file_manager::singleton().get_resource(resource_path, resource_data)) {
        log_->errorStream() << "Invalid resource to be included '" << resource_path << "', skipping directive";
        return yajl_continue;
      }

      curr_val_ = resource_data;
    }

    if (curr_subs_) {
      for (auto sub : (*curr_subs_))
        sub->set_option(curr_key_, curr_val_);
    }

    return yajl_continue;
  }

  int configurator::__on_json_map_end()
  {
    --depth_;

    if (curr_subs_)
      for (auto sub : (*curr_subs_))
        sub->on_map_end();

    if (depth_ == 1 && curr_subs_) {
      for (auto sub : (*curr_subs_))
        sub->configure();

      curr_subs_ = NULL;
      curr_ctx_.clear();
    }

    curr_key_.clear();
    curr_val_.clear();

    if (depth_ == 0) {
      return yajl_continue;
    }

    return yajl_continue;
  }

  int configurator::__on_json_array_start()
  {
    if (curr_subs_)
      for (auto sub : (*curr_subs_))
        sub->on_array_start();

    return yajl_continue;
  }

  int configurator::__on_json_array_end()
  {
    curr_key_.clear();
    curr_val_.clear();

    if (curr_subs_)
      for (auto sub : (*curr_subs_))
        sub->on_array_end();

    return yajl_continue;
  }

  bool configurator::is_reserved(const string_t& ctx)
  {
    // reserved keywords (or "contexts", in our parser sense)
    if (ctx == "include" || ctx == "include all")
      return true;

    return false;
  }
} // namespace algol
