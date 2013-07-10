/*
 * -----------------------------------------------------------------------------
 * This source file is part of OGRE
 *     (Object-oriented Graphics Rendering Engine)
 * For the latest info, see http://www.ogre3d.org/
 *
 * This source file has been incorporated into libalgol (http://www.algollabs.com)
 *
 * Copyright (c) 2000-2012 Torus Knot Software Ltd,
 *               2011-2012 algol
 *
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * -----------------------------------------------------------------------------
 */

#include "algol/plugin_manager.hpp"
#include "algol/file_manager.hpp"
#include <fstream>

namespace algol {

  plugin_manager* plugin_manager::__instance = nullptr;

  typedef void (*DLL_START_PLUGIN)(void);
  typedef void (*DLL_STOP_PLUGIN)(void);

  //-----------------------------------------------------------------------
  plugin_manager::plugin_manager() : logger("PluginManager") {

  }
  plugin_manager::~plugin_manager()
  {
    // Unload & delete resources in turn
    for( dyn_libs_t::iterator it = dyn_libs_.begin(); it != dyn_libs_.end(); ++it )
    {
        it->second->unload();
        delete it->second;
    }

    // Empty the list
    dyn_libs_.clear();
  }

  plugin_manager& plugin_manager::singleton() {
    if (!__instance)
      __instance = new plugin_manager();

    return *__instance;
  }

  void plugin_manager::init() {
    init_ = true;
  }

  void plugin_manager::shutdown() {
    init_ = false;
  }
  //-----------------------------------------------------------------------
  void plugin_manager::load_plugins( const string_t& pluginsfile )
  {
    std::list<string_t> plugin_paths;
    std::ifstream f(pluginsfile);
    while (f.is_open() && f.good()) {
      string_t l;
      f >> l;
      if (l.empty() || l == "\n")
        continue;

      log_->debugStream() << "Loading plugin: " << l << '\n';
      if (file_manager::singleton().exists(l)) {
        plugin_paths.push_back(l);
      } else {
        log_->errorStream() << "\t" << l << " seems not to exist, ignoring.\n";
      }
    }

    for (auto path : plugin_paths) {
      load_plugin(path);
    }
  }
  //-----------------------------------------------------------------------
  void plugin_manager::shutdown_plugins(void)
  {
    // NB Shutdown plugins in reverse order to enforce dependencies
    for (plugins_t::reverse_iterator i = plugins_.rbegin(); i != plugins_.rend(); ++i)
    {
      (*i)->shutdown();
    }
  }
  //-----------------------------------------------------------------------
  void plugin_manager::initialise_plugins(void)
  {
    for (plugins_t::iterator i = plugins_.begin(); i != plugins_.end(); ++i)
    {
      (*i)->initialise();
    }
  }
  //-----------------------------------------------------------------------
  void plugin_manager::unload_plugins(void)
  {
    // unload dynamic libs first
    for (plugin_libs_t::reverse_iterator i = plugin_libs_.rbegin(); i != plugin_libs_.rend(); ++i)
    {
      // Call plugin shutdown
      DLL_STOP_PLUGIN pFunc = (DLL_STOP_PLUGIN)(*i)->getSymbol("dllStopPlugin");
      // this will call uninstallPlugin
      pFunc();
      // Unload library & destroy
      unload(*i);
    }

    plugin_libs_.clear();

    // now deal with any remaining plugins that were registered through other means
    for (plugins_t::reverse_iterator i = plugins_.rbegin(); i != plugins_.rend(); ++i)
    {
      // Note this does NOT call uninstallPlugin - this shutdown is for the
      // detail objects
      (*i)->uninstall();
    }
    plugins_.clear();
  }


  void plugin_manager::install_plugin(plugin* plugin)
  {
    log_->infoStream() << "Installing plugin: " << plugin->get_name();

    plugins_.push_back(plugin);
    plugin->install();

    // if rendersystem is already initialised, call rendersystem init too
    if (init_)
    {
      plugin->initialise();
    }

    log_->infoStream() << "Plugin '" << plugin->get_name() << "' successfully installed";
  }
  //---------------------------------------------------------------------
  void plugin_manager::uninstall_plugin(plugin* plugin)
  {
    log_->infoStream() << "Uninstalling plugin: " << plugin->get_name();

    plugins_t::iterator i =
      std::find(plugins_.begin(), plugins_.end(), plugin);
    if (i != plugins_.end())
    {
      if (init_)
        plugin->shutdown();
      plugin->uninstall();
      plugins_.erase(i);
    }
    log_->infoStream() << "Plugin '" << plugin->get_name() << "' successfully uninstalled";

  }
    //-----------------------------------------------------------------------
  void plugin_manager::load_plugin(const string_t& pluginName)
  {
    // Load plugin library
    dyn_lib* lib = load( pluginName );
    // Store for later unload
    // Check for existence, because if called 2+ times dyn_libManager returns existing entry
    if (std::find(plugin_libs_.begin(), plugin_libs_.end(), lib) == plugin_libs_.end())
    {
      plugin_libs_.push_back(lib);

      // Call startup function
      DLL_START_PLUGIN pFunc = (DLL_START_PLUGIN)lib->getSymbol("dllStartPlugin");

      if (!pFunc)
        throw std::runtime_error("Cannot find symbol dllStartPlugin in library " + pluginName);

      // This must call installPlugin
      pFunc();
    }
  }
    //-----------------------------------------------------------------------
  void plugin_manager::unload_plugin(const string_t& pluginName)
  {
    plugin_libs_t::iterator i;

    for (i = plugin_libs_.begin(); i != plugin_libs_.end(); ++i)
    {
      if ((*i)->getName() == pluginName)
      {
        // Call plugin shutdown
        DLL_STOP_PLUGIN pFunc = (DLL_STOP_PLUGIN)(*i)->getSymbol("dllStopPlugin");
        // this must call uninstallPlugin
        pFunc();
        // Unload library (destroyed by dyn_libManager)
        unload(*i);
        plugin_libs_.erase(i);
        return;
      }
    }
  }

  dyn_lib* plugin_manager::load( const string_t& filename)
  {
    dyn_libs_t::iterator i = dyn_libs_.find(filename);
    if (i != dyn_libs_.end())
    {
      return i->second;
    }
    else
    {
      dyn_lib* pLib = new dyn_lib(filename);
      pLib->load();
      dyn_libs_[filename] = pLib;
      return pLib;
    }
  }
  //-----------------------------------------------------------------------
  void plugin_manager::unload(dyn_lib* lib)
  {
    dyn_libs_t::iterator i = dyn_libs_.find(lib->getName());
    if (i != dyn_libs_.end())
    {
      dyn_libs_.erase(i);
    }
    lib->unload();
    delete lib;
  }

}
