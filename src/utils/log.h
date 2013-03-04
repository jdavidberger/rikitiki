/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#pragma once

#include <iostream>
#include <map>
#include <string>
#include <stdlib.h>
#include <pthread.h>

namespace logging {

  enum LogLevels {
    INVALID = -100,
    Debug = -2,
    Verbose = -1,
    Info,
    Warning,
    Error
  };

#ifdef USE_TCLAP
#include <tclap/CmdLine.h>
  struct LogArgs {
  TCLAP::ValueArg<std::string> logFileArg;
    bool processed;
    LogArgs(TCLAP::CmdLine& cmd);
    ~LogArgs();
    void Process();
  };
#endif 

  std::map<std::string, int>& logLevels();

  void SetLogStream(const std::string& category, std::ostream& stream);
  std::ostream& LogStream(const std::string& category);

  void SetLogLevel(const std::string& category, int level);
  void SetLogLevel(const std::string& category, const std::string& level);

  inline bool ShouldLog(const std::string& category, int level){
    return logLevels()[category] <= level;
  }

  static inline std::string getCurrThreadName() {
    std::string buffer;
    buffer.resize(32);
    pthread_getname_np(pthread_self(), &buffer[0], 32);
    
    return buffer;
  }
}

#define LOG(CAT, LEVEL) if(ShouldLog(#CAT, logging::LEVEL) ) logging::LogStream(#CAT) << "[" << #CAT << ", " << #LEVEL << " (" << logging::getCurrThreadName().c_str() << ")] "
