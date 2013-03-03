#pragma once 
#include <ctemplate/template.h>
#include "connContext.h"

class ConnContext;
struct TemplatePreprocessor {
  virtual void Process(const ConnContext& ctx, 
		       ctemplate::TemplateDictionary& td)=0;
};

