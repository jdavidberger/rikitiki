#pragma once

#include <iostream>
#include <map>
#include <string>
#include <stdlib.h>
#include <pthread.h>

enum LogLevels {
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

inline bool ShouldLog(const std::string& category, int level){
  return logLevels()[category] <= level;
}

static inline char* getCurrThreadName() {
  char* buffer = (char*)malloc( sizeof(char) * 1024);
  pthread_getname_np(pthread_self(), buffer, 1024);
  return buffer;
}

#define LOG(CAT, LEVEL) if(ShouldLog(#CAT, LEVEL) ) LogStream(#CAT) << "[" << #CAT << ", " << #LEVEL << " (" << getCurrThreadName() << ")] "
