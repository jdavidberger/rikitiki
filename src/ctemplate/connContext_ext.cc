#include "connContext_ext.h"
#include <utils/config.h>

rikitiki::Response& operator <<(rikitiki::Response& response,  const ctemplate::TemplateDictionary& td){   
  ctemplate::ExpandTemplate(td.name(), ctemplate::DO_NOT_STRIP, &td, &response.response);
  return response;
}

rikitiki::ConnContext& operator<<(rikitiki::ConnContext& ctx, ctemplate::TemplateDictionary& td){
  ctx.handled = true;
  /*  if(ctx.server)
    foreach(it, ctx.server->templatePreprocessors)
    (*it)->Process(ctx, td);*/
  ctx.response << td;
  return ctx;
}

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
