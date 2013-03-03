#include "config.h"
#include <dlfcn.h>
#include <utils/log.h>
#include <assert.h>
#include <vector>
#include <stdio.h>

static void getAssemblyName(std::string& name){
    Dl_info info;
    dladdr( (void*)&getAssemblyName, &info);
    std::string exePath(info.dli_fname);
    auto start = exePath.find_last_of('/');
    auto end = exePath.find_last_of('.');

    start = start == std::string::npos ? 0 : start + 1;
    end = end == std::string::npos ? exePath.size() : end;    
    name = std::string(&exePath[start], &exePath[end]);
}

static void getAssemblyPath(std::string& path){
    Dl_info info;
    dladdr( (void*)&getAssemblyName, &info);
    std::string exePath(info.dli_fname);
    
    auto end = exePath.find_last_of('/');
    end = end == std::string::npos ? 1 : end;    
    path = std::string(&exePath[0], &exePath[end]);
}

  Configuration::Configuration() : Config() {     
    std::string exeName, exePath;
    getAssemblyName(exeName);
    getAssemblyPath(exePath);
    
    std::vector<std::string> configLocations;
    configLocations.push_back( "/etc/rikitiki/conf/" + exeName + ".cfg");
    configLocations.push_back( exePath + "/" + exeName + ".cfg");
    configLocations.push_back( exePath + "/../conf/" + exeName + ".cfg");
    configLocations.push_back( "/etc/rikitiki/conf/default.cfg");
    configLocations.push_back( exePath + "/default.cfg");
    configLocations.push_back( exePath + "/../conf/default.cfg");
    bool success = false;
    std::string filePath;
    for(auto it = configLocations.begin();it != configLocations.end();it++){
      FILE* file = fopen(it->c_str(), "r");
      if(file != NULL){
	filePath = *it;
	read(file);
	success = true;
      }
    }
    if(!success){
      std::cerr << "Could not find a config file to use. Tried: " << std::endl;
      for(auto it = configLocations.begin();it != configLocations.end();it++){
	std::cerr << "\t" << *it << std::endl;
      }
    } else {
      variables["CONFIG_PATH"] = std::string(&filePath[0], &filePath[ filePath.find_last_of('/') ]);
    }

    variables["BIN_NAME"] = exeName;
    variables["BIN_PATH"] = exePath;

  }
  Configuration& Configuration::Global(){
    static Configuration config;
    return config;
  }

bool Configuration::lookupValue (const std::string& path, const char *&value){
  return libconfig::Config::lookupValue(path.c_str(), value);
}

bool Configuration::lookupValue (const std::string& path, std::string& value){
  return libconfig::Config::lookupValue(path.c_str(), value);
}

static size_t find_close(const std::string& path, int start){
  for(size_t i = start;i < path.size();i++){
    if(path[i] == ')')
      return i;
  }
  return std::string::npos;
}

static size_t find_open_decl(const std::string& path, int start){
  bool dollar = false;
  for(size_t i = start;i < path.size();i++){
    switch( path[i] ){
    case '$': dollar = true; break;
    case '(': if(dollar) return i; 
    default:
      dollar = false;            
    }
  }
  return std::string::npos;
}

void Configuration::expand_variables(const std::string& value, std::string& exp_value){
  int last = 0;
  size_t open_decl, close_decl;

  do {
    open_decl = find_open_decl(value, last);
    close_decl =  find_close(value, open_decl);
    if(open_decl != std::string::npos){
      std::string key(&value[open_decl+1], &value[close_decl]);
      exp_value.insert(exp_value.back(), &value[last], open_decl - last - 1);
      exp_value += variables[key];    
      last = close_decl + 1;
    }
  } while (open_decl != std::string::npos);
  exp_value.insert(exp_value.size(), &value[last]);
}

bool Configuration::lookupValue (const char* path, std::string& value){
  std::string inter_value;
  bool rtn = libconfig::Config::lookupValue(path, inter_value);
  expand_variables(inter_value, value);
  return rtn;
}

bool Configuration::lookupValue (const char* path, const char *&value){
  assert(false);
}

static Configuration& __forceLoad = Configuration::Global();
