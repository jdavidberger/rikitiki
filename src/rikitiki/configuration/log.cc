#include <rikitiki/configuration/configuration>
#ifdef RT_USE_LOG
#include <rikitiki/log/log>

using namespace rikitiki::log;
using namespace libconfig;    
namespace rikitiki {
  namespace configuration {    
    static bool LoadFromConfig(Configuration& config) {
      try {
	const Setting& logsettings = config.getRoot()["log"]["levels"];
	std::string name, value;
	for(int i = 0;i < logsettings.getLength();i++){
	  const Setting& entry = logsettings[i];
	  bool cFind = entry.lookupValue("category", name);
	  bool vFind = entry.lookupValue("level", value);      
	  if(cFind && vFind)
	    SetLogLevel(name, value);
	}
	return true;
      } catch(const SettingNotFoundException& nfex){
	return false;
      }
    }
    static bool config_loaded = LoadFromConfig(Configuration::Global());
  }
}
#endif
