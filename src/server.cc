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

     void CleanConnContext(ConnContext* ctx) {
          ctx->Close();
          delete ctx;
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
                    if (handlers[i]->CanHandle(*ctx.get()))
                         handlers[i]->Handle(ctx);
               }
               catch (HandlerException& ex) {
                    LOG(Server, Debug) << "Error encountered: " << ctx->response.response.str() << std::endl;
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

     StaticContentHandler::StaticContentHandler(const std::wstring& _prefix, const std::wstring& _path) : prefix(_prefix), path(_path) {
          mime_types[L"html"] = L"text/html";
          mime_types[L"htm"] = L"text/html";
          mime_types[L"js"] = L"application/x-javascript";
          mime_types[L"css"] = L"text/css";

          mime_types[L"ico"] = L"image/x-icon";
          mime_types[L"gif"] = L"image/gif";
          mime_types[L"jpg"] = L"image/jpeg";
          mime_types[L"jpeg"] = L"image/jpeg";
          mime_types[L"png"] = L"image/png";
          mime_types[L"svg"] = L"image/svg+xml";
          mime_types[L"txt"] = L"text/plain";
          mime_types[L"xml"] = L"text/xml";
          mime_types[L"json"] = L"text/json";
          mime_types[L"xslt"] = L"application/xml";
          mime_types[L"xsl"] = L"application/xml";
          mime_types[L"ra"] = L"audio/x-pn-realaudio";
          mime_types[L"doc"] = L"application/msword";
          mime_types[L"exe"] = L"application/octet-stream";
          mime_types[L"zip"] = L"application/x-zip-compressed";
          mime_types[L"xls"] = L"application/excel";
          mime_types[L"tgz"] = L"application/x-tar-gz";
          mime_types[L"tar"] = L"application/x-tar";
          mime_types[L"gz"] = L"application/x-gunzip";
          mime_types[L"arj"] = L"application/x-arj-compressed";
          mime_types[L"rar"] = L"application/x-arj-compressed";
          mime_types[L"rtf"] = L"application/rtf";
          mime_types[L"pdf"] = L"application/pdf";
          mime_types[L"swf"] = L"application/x-shockwave-flash";
          mime_types[L"mpg"] = L"video/mpeg";
          mime_types[L"webm"] = L"video/webm";
          mime_types[L"mpeg"] = L"video/mpeg";
          mime_types[L"mp4"] = L"video/mp4";
          mime_types[L"m4v"] = L"video/x-m4v";
          mime_types[L"asf"] = L"video/x-ms-asf";
          mime_types[L"avi"] = L"video/x-msvideo";
          mime_types[L"bmp"] = L"image/bmp";

     }
     bool StaticContentHandler::Handle(ConnContextRef ctx) {
          std::wstring relpath(ctx->URI() + prefix.length());
          std::wstring mime = L"text/html";
          auto idx = relpath.rfind(L".");

          if (idx != std::wstring::npos){
               std::wstring ext = relpath.substr(idx + 1);
               mime = mime_types[ext];
               LOG(Server, Info) << mime << ", " << ext << std::endl;
          }

          relpath = path + relpath;
          LOG(Server, Info) << ctx->URI() << " -> " << relpath << std::endl;

          ctx->response.ResponseType = mime;

          std::wifstream file(relpath, std::ios::binary);
          ctx->OnHeadersFinished();
          while (file.good()) {
               auto n = (wchar_t)file.get();
               ctx->handled = true;
               if (n != (wchar_t)-1)
                    ctx->response.response << n;
          }
          return true;
     }

     bool StaticContentHandler::CanHandle(RequestContext& ctx) {
          std::wstring path(ctx.URI());
          
          if (path.find_first_of('..') != -1) {
               LOG(Server, Debug) << "Error encountered: user gave .. path." << std::endl;
               return false;
          }
          return path.size() >= prefix.size() && path.find_first_of(prefix) == 0;          
     };
     bool StaticContentHandler::visible() const {
          return false;
     }
     std::wstring StaticContentHandler::name() const {
          return prefix;
     }
     StaticContentHandler::~StaticContentHandler(){}

     std::wostream & operator<< (std::wostream & ostr,
          std::string const & str) {
          std::copy(str.begin(), str.end(),
               std::ostream_iterator<char, wchar_t>(ostr));
          return (ostr);
     }

#ifdef RT_USE_CTEMPLATE
  void Server::AddPreprocessor( rikitiki::ctemplates::TemplatePreprocessor* tp){
    templatePreprocessors.push_back(tp);
  }
#endif

}
