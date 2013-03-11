#include <rikitiki/configuration/configuration>
#ifdef RT_USE_CTEMPLATE
#include <rikitiki/ctemplate/ctemplate>
using namespace rikitiki;
static bool _setCTemplateRoot() {
  std::string root;
  bool rtn = Configuration::Global().lookupValue("ctemplate_root", root);
  LOG(Server, Verbose) << "Set ctemplate root to " << root << std::endl;
  if(rtn)
    ctemplate::Template::SetTemplateRootDirectory(root);
  else
    LOG(Server, Error) << "Could not find configuration for 'ctemplate_root'" << std::endl;
  return rtn;
};
static bool setCTemplateRoot = _setCTemplateRoot();
#endif
