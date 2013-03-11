/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include <rikitiki/ctemplate/ctemplate>
#include <rikitiki/configuration/configuration>

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
