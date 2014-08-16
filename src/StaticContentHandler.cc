#include <string>
#include <iostream>
#include <fstream>
#include <rikitiki/StaticContentHandler.h>
#include <mxcomp/utf.h>
namespace rikitiki {

     StaticContentHandler::StaticContentHandler(const std::string& _prefix, const std::string& _path) : prefix(_prefix), path(_path) {
          mime_types["html"] = "text/html";
          mime_types["htm"] = "text/html";
          mime_types["js"] = "application/x-javascript";
          mime_types["css"] = "text/css";
          mime_types["ico"] = "image/x-icon";
          mime_types["gif"] = "image/gif";
          mime_types["jpg"] = "image/jpeg";
          mime_types["jpeg"] = "image/jpeg";
          mime_types["png"] = "image/png";
          mime_types["svg"] = "image/svg+xml";
          mime_types["txt"] = "text/plain";
          mime_types["xml"] = "text/xml";
          mime_types["json"] = "text/json";
          mime_types["xslt"] = "application/xml";
          mime_types["xsl"] = "application/xml";
          mime_types["ra"] = "audio/x-pn-realaudio";
          mime_types["doc"] = "application/msword";
          mime_types["exe"] = "application/octet-stream";
          mime_types["zip"] = "application/x-zip-compressed";
          mime_types["xls"] = "application/excel";
          mime_types["tgz"] = "application/x-tar-gz";
          mime_types["tar"] = "application/x-tar";
          mime_types["gz"] = "application/x-gunzip";
          mime_types["arj"] = "application/x-arj-compressed";
          mime_types["rar"] = "application/x-arj-compressed";
          mime_types["rtf"] = "application/rtf";
          mime_types["pdf"] = "application/pdf";
          mime_types["swf"] = "application/x-shockwave-flash";
          mime_types["mpg"] = "video/mpeg";
          mime_types["webm"] = "video/webm";
          mime_types["mpeg"] = "video/mpeg";
          mime_types["mp4"] = "video/mp4";
          mime_types["m4v"] = "video/x-m4v";
          mime_types["asf"] = "video/x-ms-asf";
          mime_types["avi"] = "video/x-msvideo";
          mime_types["bmp"] = "image/bmp";

     }
     bool StaticContentHandler::Handle(ConnContextRef ctx) {
          Handler::Handle(ctx);
          std::string relpath( mxcomp::utf::convert(ctx->Request.URI() + prefix.length()));
          std::string mime = "text/html";
          auto idx = relpath.rfind(".");

          if (idx != std::wstring::npos){
               std::string ext = relpath.substr(idx + 1);
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
       std::string path( mxcomp::utf::convert( ctx.URI() ) );

          if (path.find("..") != (size_t)-1) {
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
       return mxcomp::utf::convert(prefix);
     }
     StaticContentHandler::~StaticContentHandler(){}

}
