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


