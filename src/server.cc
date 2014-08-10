/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include <rikitiki/configuration/configuration>
#include <rikitiki/rikitiki>
#include <sstream>
#ifndef _MSC_VER
#include <pthread.h>
#endif
#include <cstring>
#include <rikitiki/registration.h>
#include <fstream>

#include <rikitiki\socket.h>

namespace rikitiki {

     void CleanConnContext(ConnContext* ctx) {
          LOG(Server, Debug) << "Cleaning " << (void*)ctx << std::endl;
          if (ctx->handled)
               ctx->Close();
          delete ctx;
     }
     bool Handler::Handle(ConnContextRef ctx) {
          return ctx->handled = true;           
     }
     std::string Handler::desc() const {
          return "";
     }

     Handler::~Handler() {

     }

     Server::Server(){
          register_modules();
     }

     Server::~Server(){
          for (size_t i = 0; i < handlers.size(); i++){
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
     Handler* Server::GetHandler(Request& ctx) {
          for (size_t i = 0; i < handlers.size(); i++){
               if (handlers[i]->CanHandle(ctx))
                    return handlers[i];
          }
          return nullptr;
     }

     bool Server::Handle(ConnContextRef ctx) {
          for (size_t i = 0; i < handlers.size(); i++){
               try {
                    if (handlers[i]->CanHandle(ctx->request))
                         handlers[i]->Handle(ctx);
               }
               catch (HandlerException& ex) {
                    LOG(Server, Debug) << "Error encountered: " << ex.status << std::endl;
                    ctx << (ex.status == 0 ? HttpStatus::Internal_Server_Error : *(ex.status));
                    return true;
               }
               if (ctx->handled){
                    return true;
               }
          }
          return false;
     }     

     void Server::Register(WebModule& t){
          t.Register(*this);
     }
     
     void Server::AddHandler(Handler* handler) {
          assert(handler);
          handlers.push_back(handler);
     }

     void Server::AddHandler(Handler& handler) {
          AddHandler(&handler);
     }

#ifdef RT_USE_CTEMPLATE
  void Server::AddPreprocessor( rikitiki::ctemplates::TemplatePreprocessor* tp){
    templatePreprocessors.push_back(tp);
  }
#endif

}
