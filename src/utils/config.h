/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */
#pragma once

#include <libconfig.h++>
#include <map>

/**
   Wrap up libconfig's Config object for a few reasons.

   - Expose a global configuration. Not a giant fan of singletons but this one is useful.
   - Shadow all the 'lookupValue' methods for strings, so that we can do variable expansion.
 */
struct Configuration : public libconfig::Config {
 private:
  Configuration();
  void expand_variables(const std::string& path, std::string& value);
  std::map<std::string, std::string> variables;
 public:
  static Configuration& Global();     
  bool lookupValue (const std::string& path, const char *&value);
  bool lookupValue (const std::string& path, std::string& value);
  bool lookupValue (const char* path, std::string& value);
  bool lookupValue (const char* path, const char *&value);
};


