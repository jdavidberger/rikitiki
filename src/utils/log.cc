/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include "log.h"
#include <fstream>
#include <iostream>

static std::ostream* defaultStream = &std::cerr;
static std::map<std::string, std::ostream*> logStreams;

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

std::map<std::string, int>& logLevels(){
  static std::map<std::string, int> logLevels;
  return logLevels;
}

void SetLogStream(const std::string& category, std::ostream& stream){
  logStreams[category] = &stream;
}

std::ostream& LogStream(const std::string& category){
  std::ostream* s = logStreams[category];
  return s == 0 ? *defaultStream : *s;
}

void SetLogLevel(const std::string& category, int level){
  logLevels()[category] = level;
}
