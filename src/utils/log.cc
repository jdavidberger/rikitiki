/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include <utils/log.h>
#include <fstream>
#include <iostream>
#include <utils/config.h>
#include <cxxabi.h>
static std::ostream* defaultStream = &std::cerr;

#ifdef USE_TCLAP
LogArgs::LogArgs(TCLAP::CmdLine& cmd) : 
  logFileArg("","logFile","File to log messages too", false,"","string"),
  processed(false){
  cmd.add(logFileArg);
}

LogArgs::~LogArgs() {
  Process();
}

void LogArgs::Process(){
  if(processed) return;
  processed = true;
  std::string fn = logFileArg.getValue();
  if(fn != ""){
    defaultStream = new std::ofstream(fn);
  } 
}
#endif
namespace logging {
  std::map<std::string, int>& logLevels(){
    static std::map<std::string, int> logLevels;
    return logLevels;
  }
  using namespace libconfig;

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


  static std::map<std::string, std::ostream*> logStreams;

  static bool config_loaded = LoadFromConfig(Configuration::Global());

  void SetLogStream(const std::string& category, std::ostream& stream){
    logStreams[category] = &stream;
  }

  std::ostream& LogStream(const std::string& category){
    auto it = logStreams.find(category);
    return it == logStreams.end() ? *defaultStream : *(it->second);
  }

  void SetLogLevel(const std::string& category, int level){
    logLevels()[category] = level;
  }
  static int getLevel(const std::string& level){
    if(level == "Verbose") {
      return Verbose;
    } else if (level == "Info") {
      return Info;
    } else if (level == "Debug") {
      return Debug;
    } else if (level == "Error") {
      return Error;
    }
    return INVALID;
  }
  void SetLogLevel(const std::string& category, const std::string& level){ 
    int _level = getLevel(level);
    SetLogLevel(category, _level);
    LOG(Logging, Verbose) << "Setting '" << category << "' to '" << level << "'(" << _level << ")" << std::endl;
    if(_level == INVALID)
      LOG(Logging, Error) << "Invalid logging level('" << level << "') set for category '" << category << "'. Level set to highest verbosity." << std::endl; 
  }
}

std::ostream& operator<<(std::ostream& stream, const std::type_info& ti){
  char buf[1024];
  size_t size=1024;
  int status;
  char* res = abi::__cxa_demangle (ti.name(),
				   buf,
				   &size,
				   &status);
  return stream << res;
}
