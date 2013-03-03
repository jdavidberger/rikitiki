/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include "log.h"
#include <fstream>
#include <iostream>
#include <config.h>

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
  static bool LoadFromConfig() {
    try {
      const Setting& logsettings = Configuration::Global().getRoot()["log"]["levels"];
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

  static std::map<std::string, std::ostream*>& logStreams(){
    static std::map<std::string, std::ostream*>* logStreams = 0;
    if(logStreams == 0){
      logStreams = new std::map<std::string, std::ostream*>();
      LoadFromConfig();
    }
    return *logStreams;
  }
  
  static std::map<std::string, std::ostream*>& _logStreams = logStreams(); // force init

  void SetLogStream(const std::string& category, std::ostream& stream){
    logStreams()[category] = &stream;
  }

  std::ostream& LogStream(const std::string& category){
    std::ostream* s = logStreams()[category];
    return s == 0 ? *defaultStream : *s;
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
