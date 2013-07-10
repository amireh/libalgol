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

#ifndef H_ALGOL_PLUGIN_MANAGER_H

#include <algol/logger.hpp>
#include "algol/dyn_lib.hpp"
#include "algol/plugin.hpp"

#include <vector>
#include <map>

namespace algol {

  typedef std::vector<dyn_lib*> plugin_libs_t;
  typedef std::vector<plugin*> plugins_t;

  /**
   * \addtogroup Core
   * @{
   * @class plugin_manager
   * Manages client application plug-ins and provides an interface
   * for (un)loading them.
   */
  class plugin_manager : public logger {
  public:

    static plugin_manager& singleton();

    plugin_manager();
    virtual ~plugin_manager();

    /** Initializes the Plugin system.
     *
     * @warning
     *  The plugin manager should not be used before initializing.
     */
    void init();

    /** Frees any resources allocated by the Plugins system.
     *
     * @warning
     *  The plugin manager should not be used after shutting down.
     */
    void shutdown();

    /** Loads all plugins found in @filepath */
    void load_plugins(const string_t& filepath);

    /** Initializes all loaded plugins */
    void initialise_plugins();

    /** Shuts down all loaded plugins */
    void shutdown_plugins();

    /** Unloads all loaded plugins */
    void unload_plugins();

    /**
     * Manually load a Plugin contained in a DLL / DSO.
     *
     * @remarks
     *  Plugins embedded in DLLs can be loaded at startup using the plugin
     *  configuration file specified when you create Root (default: plugins.cfg).
     *  This method allows you to load plugin DLLs directly in code.
     *  The DLL in question is expected to implement a dllStartPlugin
     *  method which instantiates a Plugin subclass and calls Root::installPlugin.
     *  It should also implement dllStopPlugin (see Root::unloadPlugin)
     *
     * @param pluginName Name of the plugin library to load
     */
    void load_plugin(const string_t& name);

    /**
     * Manually unloads a Plugin contained in a DLL / DSO.
     *
     * @remarks
     *  Plugin DLLs are unloaded at shutdown automatically. This method
     *  allows you to unload plugins in code, but make sure their
     *  dependencies are decoupled first. This method will call the
     *  dllStopPlugin method defined in the DLL, which in turn should call
     *  Root::uninstallPlugin.
     * @param pluginName Name of the plugin library to unload
    */
    void unload_plugin(const string_t& name);

    /**
     * Install a new plugin.
     *
     * @remarks
     *   This installs a new extension to OGRE. The plugin itself may be loaded
     *   from a DLL / DSO, or it might be statically linked into your own
     *   application. Either way, something has to call this method to get
     *   it registered and functioning. You should only call this method directly
     *   if your plugin is not in a DLL that could otherwise be loaded with
     *   loadPlugin, since the DLL function dllStartPlugin should call this
     *   method when the DLL is loaded.
    */
    void install_plugin(plugin* plugin);

    /**
     * Uninstall an existing plugin.
     *
     * @remarks
     *   This uninstalls an extension to OGRE. Plugins are automatically
     *   uninstalled at shutdown but this lets you remove them early.
     *   If the plugin was loaded from a DLL / DSO you should call unloadPlugin
     *   which should result in this method getting called anyway (if the DLL
     *   is well behaved).
    */
    void uninstall_plugin(plugin* plugin);

    /** Gets a read-only list of the currently installed plugins. */
    const plugins_t& installed_plugins() const { return plugins_; }

  protected:
    static plugin_manager* __instance;

    typedef std::map<string_t, dyn_lib*> dyn_libs_t;

    plugin_libs_t plugin_libs_; /// List of plugin DLLs loaded
    plugins_t     plugins_; /// List of Plugin instances registered
    dyn_libs_t    dyn_libs_;

    bool init_;

  protected:
    /**
     * Loads the passed library.
     * @param
     * filename The name of the library. The extension can be omitted
     */
    dyn_lib* load(const string_t& filename);

    /**
     * Unloads the passed library.
     * @param
     * filename The name of the library. The extension can be omitted
     */
    void unload(dyn_lib* lib);
  };

  /** @} */

}

#endif