/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include <rikitiki/log/log.h>
#include <fstream>
#include <iostream>
#include <rikitiki/configuration/configuration>
#include <cxxabi.h>
static std::ostream* defaultStream = &std::cerr;


namespace rikitiki {
  namespace log {
  std::map<std::string, int> logLevels;
  static std::map<std::string, std::ostream*> logStreams;

  void SetLogStream(const std::string& category, std::ostream& stream){
    logStreams[category] = &stream;
  }

  std::ostream& LogStream(const std::string& category){
    auto it = logStreams.find(category);
    return it == logStreams.end() ? *defaultStream : *(it->second);
  }

  void SetLogLevel(const std::string& category, int level){
    logLevels[category] = level;
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
}

