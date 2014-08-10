#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <rikitiki\http\Request.h>
#include <rikitiki\http\content_types.h>
#include <rikitiki\http\http_statuses.h>
#include <mxcomp\useful_macros.h>
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
          void reset();

          std::mutex payloadWrite;

          virtual void SetStartline(const std::wstring&) OVERRIDE;
          virtual std::wstring Startline() const OVERRIDE;

          Response& operator <<(const rikitiki::HttpStatus& t);
          using Message::operator<<;          
     };
     struct IResponse : public virtual IMessage, public virtual Response {

     };

     struct OResponse : public virtual OMessage {
          virtual OResponse& operator <<(const rikitiki::HttpStatus& t) = 0;
          virtual OResponse& operator <<(rikitiki::ContentType::t t) OVERRIDE;
          virtual OResponse& operator <<(const rikitiki::Cookie& t) OVERRIDE;
          virtual OResponse& operator <<(const rikitiki::Header& t) OVERRIDE;
     };
     struct OResponseMemory : public OResponse, public virtual Response {
          virtual OResponseMemory& operator <<(const rikitiki::HttpStatus& t);
          virtual OResponseMemory& operator <<(const rikitiki::Header &);
          virtual OResponseMemory& operator <<(const rikitiki::Cookie &);
          virtual OResponseMemory& operator <<(rikitiki::ContentType::t);

          virtual size_t WritePayloadData(const char*, size_t) OVERRIDE;
          virtual void WriteHeader(const Header&) OVERRIDE;
     };
     struct OResponseWriter : public OResponse, public OMessageWriter {
          const HttpStatus* status = &HttpStatus::OK;

          virtual size_t WriteStartLine();
          
          virtual OResponseWriter& operator <<(const rikitiki::HttpStatus& t);
     };


     template <class T> auto operator <<(OResponse& out, const T& obj) -> decltype(std::declval<std::stringstream>() << obj, std::declval<OResponse&>())
     {
          std::stringstream ss;
          auto buff = mxcomp::attachContinousBuffer(ss);
          ss << obj;
          out.WritePayloadData(buff->data(), buff->length());
          return out;
     }

     

     /* Build a response object from a raw string of bytes. */
     class ResponseBuilder  : public BufferedReader {          
          MessageParserState state;
          
          virtual bool OnBufferedData(const char*, std::size_t length);
     public:
          Response* response;
          ResponseBuilder(Response*);        
          virtual ~ResponseBuilder(){}
     };
}
#ifdef _MSC_VER
#undef decltype
#endif