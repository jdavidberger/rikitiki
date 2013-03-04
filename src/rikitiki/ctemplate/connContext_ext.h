#include "../connContext.h"
#include "template_emitter_ext.h"

rikitiki::Response& operator <<(rikitiki::Response& response,  const ctemplate::TemplateDictionary& td); 
rikitiki::ConnContext& operator<<(rikitiki::ConnContext& ctx, ctemplate::TemplateDictionary& td);
