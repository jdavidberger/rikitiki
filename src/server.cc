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

namespace rikitiki {

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
     Handler* Server::GetHandler(RequestContext& ctx) {
          for (size_t i = 0; i < handlers.size(); i++){
               if (handlers[i]->CanHandle(ctx))
                    return handlers[i];
          }
          return nullptr;
     }

     bool Server::Handle(ConnContextRef ctx) {
          for (size_t i = 0; i < handlers.size(); i++){
               try {
                    handlers[i]->Handle(ctx);
               }
               catch (HandlerException& ex) {
                    //	LOG(Server, Debug) << "Error encountered: " << ctx->response.response.str() << std::endl;
                    ctx << (ex.status == 0 ? HttpStatus::Internal_Server_Error : *(ex.status));
                    return true;
               }
               if (ctx->handled){
                    return true;
               }
          }
          return false;
     }

     void Server::AddHandler(Handler* handler) {
          assert(handler);
          handlers.push_back(handler);
     }

     void Server::AddHandler(Handler& handler) {
          AddHandler(&handler);
     }

     StaticContentHandler::StaticContentHandler(const std::wstring& _prefix, const std::wstring& _path) : prefix(_prefix), path(_path) {}
     bool StaticContentHandler::Handle(ConnContextRef ctx) {
          std::wstring relpath(ctx->URI() + prefix.length());
          relpath = path + relpath;
          std::wifstream file(relpath);
          while (file.good()) {
               ctx->response.response << file.get();
          }
          return true;
     }

     bool StaticContentHandler::CanHandle(RequestContext& ctx) {
          std::wstring path(ctx.URI());
          if (path.find_first_of('..') >= 0)
               return false;
          return path.find_first_of(prefix) == 0;          
     };
     bool StaticContentHandler::visible() const {
          return false;
     }
     std::wstring StaticContentHandler::name() const {
          return prefix;
     }
     StaticContentHandler::~StaticContentHandler(){}
  
#ifdef RT_USE_CTEMPLATE
  void Server::AddPreprocessor( rikitiki::ctemplates::TemplatePreprocessor* tp){
    templatePreprocessors.push_back(tp);
  }
#endif

}
