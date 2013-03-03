/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#pragma once 
#include <ctemplate/template.h>
#include "connContext.h"

class ConnContext;
struct TemplatePreprocessor {
  virtual void Process(const ConnContext& ctx, 
		       ctemplate::TemplateDictionary& td)=0;
};

