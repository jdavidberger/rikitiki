/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include <rikitiki/ctemplate/ctemplate>
#include <rikitiki/configuration/configuration>

namespace rikitiki {
  rikitiki::Response& operator <<(rikitiki::Response& response,  const ctemplate::TemplateDictionary& td){   
    ctemplate::ExpandTemplate(td.name(), ctemplate::DO_NOT_STRIP, &td, &response.response);
    return response;
  }

  rikitiki::ConnContext& operator<<(rikitiki::ConnContext& ctx, ctemplate::TemplateDictionary& td){
    ctx.handled = true;
    if(ctx.server)
      for(auto it : ctx.server->templatePreprocessors)
	it->Process(ctx, td);
    ctx.response << td;
    return ctx;
  }

#include <rikitiki/configuration/configuration>
  using namespace rikitiki;
  void register_ctemplate() {
#ifdef RT_USE_CONFIGURATION
    std::string root;
    bool found = Configuration::Global().lookupValue("ctemplate_root", root);
    LOG(Server, Verbose) << "Set ctemplate root to " << root << std::endl;
    if(found)
      ctemplate::Template::SetTemplateRootDirectory(root);
    else
      LOG(Server, Error) << "Could not find configuration for 'ctemplate_root'" << std::endl;
#endif
  };

}
