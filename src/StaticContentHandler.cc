#include <string>
#include <iostream>
#include <fstream>
#include <rikitiki\StaticContentHandler.h>
namespace rikitiki {

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
          Handler::Handle(ctx);
          std::wstring relpath(ctx->Request.URI() + prefix.length());
          std::wstring mime = L"text/html";
          auto idx = relpath.rfind(L".");

          if (idx != std::wstring::npos){
               std::wstring ext = relpath.substr(idx + 1);
               mime = mime_types[ext];
               LOG(Server, Info) << mime << ", " << ext << std::endl;
          }

          relpath = path + relpath;
          LOG(Server, Info) << ctx->Request.URI() << " -> " << relpath << std::endl;

          ctx->Response << ContentType::FromString(mime);          

          std::ifstream file(relpath, std::ios::binary);
          //ctx->OnHeadersFinished();

          if (file.good()) {
               ctx->handled = true;
               ctx->Response << file.rdbuf();
               return true;
          }

          return false;
     }

     bool StaticContentHandler::CanHandle(Request& ctx) {
          std::wstring path(ctx.URI());

          if (path.find(L"..") != -1) {
               LOG(Server, Debug) << "Error encountered: user gave .. path." << std::endl;
               return false;
          }

          auto prefixStart = path.find(prefix);
          return path.size() >= prefix.size() && prefixStart == 0;
     };
     bool StaticContentHandler::visible() const {
          return false;
     }
     std::wstring StaticContentHandler::name() const {
          return prefix;
     }
     StaticContentHandler::~StaticContentHandler(){}

}