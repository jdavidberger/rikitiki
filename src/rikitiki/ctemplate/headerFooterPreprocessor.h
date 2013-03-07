/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */
#pragma once
#include "templatePreprocessor.h"

namespace rikitiki {
  namespace ctemplates {
    struct HeaderFooterPreprocessor : public TemplatePreprocessor {
      std::string header_tpl, footer_tpl, page_header_tpl, page_footer_tpl;
      HeaderFooterPreprocessor();
      virtual void Process(const ConnContext& ctx, 
			   ctemplate::TemplateDictionary& td);
    };
  }
}
