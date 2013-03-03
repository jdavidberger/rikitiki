#include "connContext_ext.h"

rikitiki::Response& operator <<(rikitiki::Response& response,  const ctemplate::TemplateDictionary& td){   
  ctemplate::ExpandTemplate(td.name(), ctemplate::DO_NOT_STRIP, &td, &response.response);
  return response;
}

rikitiki::ConnContext& operator<<(rikitiki::ConnContext& ctx, ctemplate::TemplateDictionary& td){
  ctx.handled = true;
  if(ctx.server)
    foreach(it, ctx.server->templatePreprocessors)
      (*it)->Process(ctx, td);
  ctx.response << td;
  return ctx;
}
