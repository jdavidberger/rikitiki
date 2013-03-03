/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include "server.h"
#include "mongoose/mongoose.h"
#include <sstream>
#include <pthread.h>
#include "connContext.h"
#include "mongoose/connContext.h"
#include <config.h>

namespace rikitiki {
  struct HeaderFooterPreprocessor : public TemplatePreprocessor {
    virtual void Process(const ConnContext& ctx, 
			 ctemplate::TemplateDictionary& td){    
      auto header = td.AddIncludeDictionary("HEADER");
      header->SetFilename("header.tpl");

      auto page_header = td.AddIncludeDictionary("PAGE_HEADER");
      page_header->SetFilename("page_header.tpl");
      for(unsigned int i = 0;i < ctx.server->handlers.size();i++){
	ctemplate::TemplateDictionary* row = page_header->AddSectionDictionary("MENUITEM");
	row->SetValue("name", ctx.server->handlers[i]->name());
	row->SetValue("link", ctx.server->handlers[i]->name());
      }
    
      auto footer = td.AddIncludeDictionary("FOOTER");
      footer->SetFilename("footer.tpl");

      auto page_footer = td.AddIncludeDictionary("PAGE_FOOTER");
      page_footer->SetFilename("page_footer.tpl");
    }
  };

  std::string Handler::desc() const {
    return "";
  }


  std::string CommandHandler::name() const {
    return command;
  }
  CommandHandler::CommandHandler(std::string _command, std::string _desc) : 
    command(_command),
    handleChildren(true)
  {
  
  } 

  bool CommandHandler::Handle(ConnContext& ctx){
    bool shouldAttempt = handleChildren ? 
      strncmp(command.c_str(), ctx.URI(), command.length()) == 0 :
      strcmp(command.c_str(), ctx.URI()) == 0;
    
    if(shouldAttempt){
      Process(ctx);
      return true;
    }
    return false;
  }

  struct GHandler : public Handler {
    Server::handle_t handlef;
    GHandler(  Server::handle_t _handlef) : handlef(_handlef) {}
    virtual bool Handle(ConnContext& ctx) {
      return handlef(ctx);
    }
    virtual std::string name() const {
      return "anonymous";
    }
  };

  struct IndexPage : public CommandHandler {
    Server* server;
    IndexPage(Server* s) : server(s), CommandHandler("/") {
      handleChildren = false;
    }
    void Process(ConnContext& ctx) {
      ctemplate::TemplateDictionary td("commands.tpl");
      for(unsigned int i = 0;i < server->handlers.size();i++){
	ctemplate::TemplateDictionary* row = td.AddSectionDictionary("HANDLER");
	row->SetValue("name", server->handlers[i]->name());
	row->SetValue("description", server->handlers[i]->desc());
      }
      ctx << td;  
    }
  };

  Server::Server(int _port) : port(_port) {
    //    AddHandler(new IndexPage(this) );
    AddPreprocessor(new HeaderFooterPreprocessor());
  }

  void* Server::Handle(enum mg_event event, struct mg_connection *conn) {
    if(event != MG_NEW_REQUEST)
      return (void*)0;

    mongoose::MongooseConnContext ctx(this, event, conn);
    for(unsigned int i = 0;i < handlers.size();i++){
      handlers[i]->Handle(ctx);
      if(ctx.handled){
	ctx.writeResponse();
	return (void*)1;
      }
    }
    return (void*)0;
  }

  void Server::AddPreprocessor( TemplatePreprocessor* tp){
    templatePreprocessors.push_back(tp);
  }

  void Server::AddHandler( Handler* handler) {
    handlers.push_back(handler);
  }

  void Server::AddHandler( Handler& handler) {
    AddHandler(&handler);
  }

  void Server::AddHandler( handle_t handler) {
    AddHandler( new GHandler( handler ) );
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
}
