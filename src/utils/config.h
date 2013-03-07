/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */
#pragma once

#include <libconfig.h++>
#include <map>

struct Configuration : public libconfig::Config {
 private:
  Configuration();
 public:
  std::map<std::string, std::string> variables;

  static Configuration& Global();    
  void expand_variables(const std::string& path, std::string& value);
  bool lookupValue (const std::string& path, const char *&value);
  bool lookupValue (const std::string& path, std::string& value);
  bool lookupValue (const char* path, std::string& value);
  bool lookupValue (const char* path, const char *&value);
};


