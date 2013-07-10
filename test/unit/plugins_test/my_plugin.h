#include <algol/algol.hpp>
#include <algol/plugin.hpp>
#include <algol/logger.hpp>

using namespace algol;
 
class MyPlugin : public plugin, public logger
{
  public:
   MyPlugin();
 
    const string_t& get_name() const;
    void install();
    void initialise();
    void shutdown();
    void uninstall();
};