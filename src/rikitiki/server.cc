/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include "server.h"
#include "mongoose/mongoose.h"
#include <sstream>
#include <pthread.h>
#include "connContext.h"
#include "mongoose/connContext.h"
#include <utils/config.h>
#include <cstring>

namespace rikitiki {

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
    virtual bool visible() const {
      return false;
    }
    virtual std::string name() const {
      return "anonymous";
    }
  };

  bool Server::Handle(ConnContext& ctx) {
    for(size_t i = 0;i < handlers.size();i++){
      LOG(Server, Debug) << "Trying handler " << handlers[i]->name() << std::endl;
      handlers[i]->Handle(ctx);      
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

}
