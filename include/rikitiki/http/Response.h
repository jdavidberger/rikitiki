#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <rikitiki/http/Request.h>
#include <rikitiki/http/content_types.h>
#include <rikitiki/http/http_statuses.h>
#include <mxcomp/useful_macros.h>
#include <utility>
#pragma warning (disable: 4265)
#include <mutex>
#pragma warning (default: 4265)

#ifdef _MSC_VER
//#define decltype(a,b) b
#endif
namespace rikitiki {

     std::ostream& operator <<(std::ostream& response, const wchar_t* obj);
     std::ostream& operator <<(std::ostream& response, const std::wstring& obj);

     std::wostream& operator <<(std::wostream& response, const std::string& obj);

     /**
     Response class that handlers write to. Contains headers, response stream, status, etc.
     */
     struct Response : public virtual Message {
     private:
          CookieCollection cookies; 
           HeaderCollection headers;
           ByteStream body;
     public:
          Response();
          ~Response();
          
          virtual HeaderCollection& Headers() OVERRIDE{
               return headers;
          };
          virtual CookieCollection& Cookies() OVERRIDE{
               return cookies;
          };

          virtual ByteStream& Body() OVERRIDE {
               return body;
          }
          
          const HttpStatus* status;

          std::mutex payloadWrite;

          virtual void SetStartline(const std::wstring&) OVERRIDE;
          virtual std::wstring Startline() const OVERRIDE;

          Response& operator <<(const rikitiki::HttpStatus& t);
          using Message::operator<<;          
     };
 
}
