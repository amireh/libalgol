#include "my_plugin.h"
const string_t sPluginName = "MyPlugin";
//---------------------------------------------------------------------
MyPlugin::MyPlugin() : logger("MyPlugin")
{
}
//---------------------------------------------------------------------
const string_t& MyPlugin::get_name() const
{
  return sPluginName;
}
//---------------------------------------------------------------------
void MyPlugin::install()
{
  //install called - create stuff here
  log_->infoStream() << "installed!";
}
//---------------------------------------------------------------------
void MyPlugin::initialise()
{
  //intialise called - register stuff here
  log_->infoStream() << "initialized!";
}
//---------------------------------------------------------------------
void MyPlugin::shutdown()
{
  //shutdown called - unregister stuff here
  log_->infoStream() << "shutting down!";
}
//---------------------------------------------------------------------
void MyPlugin::uninstall()
{
  //uninstall called - delete stuff here
  log_->infoStream() << "uninstalled!";
}