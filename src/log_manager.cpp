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

#include "algol/log_manager.hpp"
#include "algol/utility.hpp"
#include "algol/file_manager.hpp"
#include <map>

namespace algol {

  log_manager* log_manager::__instance = 0;
  bool log_manager::silent_ = false;

  void log_manager::silence() {
    silent_ = true;
  }

  log_manager::log_manager()
  : configurable(),
    log_appender_(0),
    log_layout_(0),
    log_category_(0),
    log_(0),
    anonymous_log_(0),
    category_name_("algol")
  {
    config_.log_device = "stdout";
    config_.log_dir = "log";
    config_.log_name = algol_app().name + ".log";
    config_.log_level = "debug";
    config_.log_filesize = "10M";
    config_.app_name = algol_app().name;
    config_.app_version = algol_app().version;
    config_.silent = false;

    init();
  }

  log_manager::~log_manager()
  {
  }

  log_manager& log_manager::singleton() {
    if (!__instance)
      __instance = new log_manager();

    return *__instance;
  }

  string_t const& log_manager::category() {
    return category_name_;
  }

  void log_manager::init(string_t cat) {
    category_name_ = cat;

    if (log_) // we already initialized
      return;

    log_category_ = &log4cpp::Category::getInstance(category_name_);
    log_category_->setAdditivity(false);
    log_category_->setPriority(log4cpp::Priority::DEBUG);

    log_
      = new log4cpp::FixedContextCategory(category_name_, "log_manager");
    anonymous_log_
      = new log4cpp::FixedContextCategory(category_name_, "anonymous");
  }

  void log_manager::cleanup()
  {
    if (log_)
      delete log_;
    if (anonymous_log_)
      delete anonymous_log_;

    // shutting down the log4cpp category will take care of freeing the
    // appenders and their layouts, so we don't have to deal with it
    log_appender_->close();
    log_category_->removeAppender(log_appender_);
    log_appender_ = 0;
    log_category_ = 0;
    log_layout_ = 0;
    log_ = anonymous_log_ = 0;

    log4cpp::Category::shutdown();
  }

  log_manager::log_t* log_manager::log()
  {
    return anonymous_log_;
  }

  void log_manager::configure()
  {
    // set the logging level
    std::map< string_t, log4cpp::Priority::Value > str2prio;
    str2prio.insert(std::make_pair("debug",  log4cpp::Priority::DEBUG));
    str2prio.insert(std::make_pair("notice", log4cpp::Priority::NOTICE));
    str2prio.insert(std::make_pair("info",   log4cpp::Priority::INFO));
    str2prio.insert(std::make_pair("warn",   log4cpp::Priority::WARN));
    str2prio.insert(std::make_pair("error",  log4cpp::Priority::ERROR));

    if (str2prio.find(config_.log_level) == str2prio.end()) {
      std::cerr << "invalid logging level '" << config_.log_level << "', falling back to DEBUG\n";

      config_.log_level = "debug";
    }

    log_category_->setPriority(str2prio[config_.log_level]);

    // remove the current appender
    if (log_appender_) {
      log_appender_->close();
      log_category_->removeAppender(log_appender_);
      log_appender_ = 0;
    }

    // create the new appender
    bool using_syslog = false;
    if (config_.log_device == "syslog")
    {
      log_appender_ =	new log4cpp::SyslogAppender("SyslogAppender", algol_app().name);
      using_syslog = true;
    }
    else if (config_.log_device == "stdout")
    {
      assert(!log_appender_);
      log_appender_ =	new log4cpp::OstreamAppender("STDOUTAppender", &std::cout);
    }
    else
    {
      uint64_t filesz = 0;
      if (!utility::string_to_bytes(config_.log_filesize, &filesz)) {
        std::cerr << "invalid log filesize '" << config_.log_filesize << ", defaulting to 10M\n";
        filesz = 10 * 1024 * 1024;
      }

      // make sure the directory exists
      file_manager& fmgr = file_manager::singleton();
      path_t log_path = fmgr.root_path() / path_t(config_.log_dir);
      if (!boost::filesystem::exists(log_path))
        boost::filesystem::create_directory(log_path);

      log_appender_ =
      new log4cpp::RollingFileAppender(
        "RollingFileAppender",
        log_path.make_preferred().string() + "/" + config_.log_name,
        filesz);
    }

    // register the appender
    log_category_->addAppender(log_appender_);

    // assign a vanilla appender layout for header logging
    if (!config_.silent) {
      log_layout_ = new algol_vanilla_log_layout();
      log_appender_->setLayout(log_layout_);

      size_t hlen = 78; // number of characters in the header excluding the margins

      log_->errorStream() << '\n';
      log_->errorStream() << '+';
      for (size_t i = 0; i < hlen / 2; ++i)
        log_->errorStream() << "+-";
      log_->errorStream() << '+';

      // log application name and version
      log_->errorStream() << "\n+";
      {
        size_t trail = 0;
        size_t nr_pads = (hlen - (algol_app().fqn.size() + 2)) / 2;
        if (nr_pads % 2 != 0) trail = 1;

        for (size_t i = 0; i < nr_pads; ++i)
          log_->errorStream() << ' ';

          // log_->errorStream() << config_.app_name << ' ' << config_.app_version;
          log_->errorStream() << algol_app().fqn;

        for (size_t i = 0; i < nr_pads+trail; ++i)
          log_->errorStream() << ' ';

        log_->errorStream() << '+';
      }

      // log application website
      log_->errorStream() << "\n+";
      {
        size_t trail = 0;
        size_t nr_pads = (hlen - (config_.app_website.size() + 2 /* ( ) */)) / 2;
        if (nr_pads % 2 != 0) trail = 1;

        for (size_t i = 0; i < nr_pads; ++i)
          log_->errorStream() << ' ';

          log_->errorStream() << '(' << config_.app_website << ')';

        for (size_t i = 0; i < nr_pads + trail; ++i)
          log_->errorStream() << ' ';

        log_->errorStream() << '+';
      }

      log_->errorStream() << "\n+";
      for (size_t i = 0; i < hlen / 2; ++i)
        log_->errorStream() << "+-";
      log_->errorStream() << "+\n";

      log_appender_->close();
      delete log_layout_;
    }

    // assign the real appender layout
    if (using_syslog)
      log_layout_ = new algol_syslog_log_layout();
    else
      log_layout_ = new algol_file_log_layout();

    log_appender_->setLayout(log_layout_);
    log_appender_->reopen();

    if (!silent_) {
      log_->getStream(str2prio[config_.log_level]) << "logging level set to " << config_.log_level;

      if (config_.log_device == "file") {
        log_->getStream(str2prio[config_.log_level])
        << "log file will be rotated every " << config_.log_filesize;
      }
    }

  }

  void log_manager::set_option(string_t const& key, string_t const& value)
  {
    if (key == "log_device" || key == "log interface" || key == "log device" || key == "device")
    {
      if (value == "syslog")
        config_.log_device = value;
      else if (value == "stdout")
        config_.log_device = value;
      else if (value == "file")
        config_.log_device = value;
      else {
        log_->warnStream() << "unknown logging mechanism specified '" << value << "', falling back to 'file'";
        config_.log_device = "file";
      }
    }
    else if (key == "log_filesize" || key == "log filesize") {
      config_.log_filesize = value;
    }
    else if (key == "log_name" || key == "log filename") {
      config_.log_name = value;
    }
    else if (key == "log_dir" || key == "log directory") {
      config_.log_dir = value;
    }
    else if (key == "log_level" || key == "log level") {
      config_.log_level = value;
    }
    // else if (key == "app_name" || key == "app name") {
    //   config_.app_name = value;
    // }
    // else if (key == "app_version" || key == "app version") {
    //   config_.app_version = value;
    // }
    else if (key == "app_website" || key == "app website") {
      config_.app_website = value;
    }
    else if (key == "log header") {
      config_.silent = (value == "false") ? true : false;
    }
    else {
      log_->warnStream() << "unknown log_manager config setting '" << key << "' => '" << value << "', discarding";
    }
  }
} // namespace algol
