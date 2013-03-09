/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include <rikitiki/ctemplate/ctemplate.h>
#include <rikitiki/rikitiki.h>

using namespace rikitiki;
namespace rikitiki {
  namespace ctemplates {
    HeaderFooterPreprocessor::HeaderFooterPreprocessor(){
      header_tpl = "header.tpl";
      footer_tpl = "footer.tpl";
      page_header_tpl = "page_header.tpl";
      page_footer_tpl = "page_footer.tpl";
    }
    void HeaderFooterPreprocessor::Process(const ConnContext& ctx, 
					   ctemplate::TemplateDictionary& td){    
      auto header = td.AddIncludeDictionary("HEADER");
      header->SetFilename( header_tpl );

      auto page_header = td.AddIncludeDictionary("PAGE_HEADER");
      page_header->SetFilename("page_header.tpl");
      for(unsigned int i = 0;i < ctx.server->handlers.size();i++){
	if(ctx.server->handlers[i]->visible()){
	  ctemplate::TemplateDictionary* row = page_header->AddSectionDictionary("MENUITEM");
	  row->SetValue("name", ctx.server->handlers[i]->name());
	  row->SetValue("link", ctx.server->handlers[i]->name());
	}
      }
    
      auto footer = td.AddIncludeDictionary("FOOTER");
      footer->SetFilename("footer.tpl");

      auto page_footer = td.AddIncludeDictionary("PAGE_FOOTER");
      page_footer->SetFilename("page_footer.tpl");
    }
  }
}    
