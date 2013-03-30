#include <rikitiki/stencet/stencet>
#include <rikitiki/configuration/configuration>
#include <rikitiki/log/log>
#ifdef RT_USE_STENCET
namespace rikitiki {
void register_stencet(){
  #ifdef RT_USE_CONFIGURATION
    std::string root;
    bool found = Configuration::Global().lookupValue("stencet_root", root);
    LOG(Server, Verbose) << "Set stencet root to " << root << std::endl;
    if(found)
      stencet::Template::AddDirectory(root);

#endif

  }
}
#endif
