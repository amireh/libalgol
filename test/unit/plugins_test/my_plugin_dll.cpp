#include "my_plugin.h"
#include <algol/plugin_manager.hpp>
 
//global variable
MyPlugin* myPlugin;
 
//-----------------------------------------------------------------------
extern "C" void dllStartPlugin(void)
{
  // Create plugin
  myPlugin = new MyPlugin();
 
  // Register
  algol::plugin_manager::singleton().install_plugin(myPlugin);
}
extern "C" void dllStopPlugin(void)
{
  algol::plugin_manager::singleton().uninstall_plugin(myPlugin);
  delete myPlugin;
}