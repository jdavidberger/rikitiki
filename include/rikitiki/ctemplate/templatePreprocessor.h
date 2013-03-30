/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#pragma once 
#include <ctemplate/template.h>

namespace rikitiki {
  class ConnContext;
  namespace ctemplates {
    /** 
	Template preprocessors are ran directly prior to template expansion. 
	Good place to add global elements like menus and other assets
     */
    struct TemplatePreprocessor {
      virtual void Process(const ConnContext& ctx, 
			   ::ctemplate::TemplateDictionary& td)=0;
      virtual ~TemplatePreprocessor();
    };
  }
}
