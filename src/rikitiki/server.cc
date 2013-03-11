/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include <rikitiki/configuration/configuration>
#include <rikitiki/rikitiki>
#include <sstream>
#include <pthread.h>
#include <cstring>

namespace rikitiki {

  std::string Handler::desc() const {
    return "";
  }
  
  Handler::~Handler() {

  }
  /**
     Wraps up a function into a handler
   */
  struct GHandler : public Handler {
    Server::handle_t handlef;
    GHandler(  Server::handle_t _handlef) : handlef(_handlef) {}
    virtual bool Handle(ConnContext& ctx) {
      return handlef(ctx);
    }
    virtual bool visible() const {
      return false;
    }
    virtual std::string name() const {
      return "anonymous";
    }
  };

  Server::~Server(){
    for(size_t i = 0;i < handlers.size();i++){
      delete handlers[i];
      handlers[i] = 0;
    }

#ifdef RT_USE_CTEMPLATE
    for(size_t i = 0;i < templatePreprocessors.size();i++){
    delete templatePreprocessors[i];
    templatePreprocessors[i] = 0;
  }
    ctemplate::mutable_default_template_cache()->ClearCache();
#endif
    
  }

  bool Server::Handle(ConnContext& ctx) {
    for(size_t i = 0;i < handlers.size();i++){
      try {
	handlers[i]->Handle(ctx);      
      } catch(HandlerException& ex) {
	LOG(Server, Error) << ctx.response.response.str() << std::endl;
	ctx << (ex.status == 0 ? HttpStatus::Internal_Server_Error : *(ex.status));
	ctx.writeResponse(); // Assumably the thrower cleared then wrote out a reason to throw. 
	return true;
      }
      if(ctx.handled){
	ctx.writeResponse();
	return true;
      }
    }
    return false;
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

#ifdef RT_USE_CTEMPLATE
  void Server::AddPreprocessor( rikitiki::ctemplates::TemplatePreprocessor* tp){
    templatePreprocessors.push_back(tp);
  }
#endif

}
