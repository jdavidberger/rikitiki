#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <rikitiki\http\Request.h>
#include <rikitiki\http\content_types.h>
#include <rikitiki\http\http_statuses.h>
#include <mxcomp\useful_macros.h>

#pragma warning (disable: 4265)
#include <mutex>
#pragma warning (default: 4265)

#ifdef _MSC_VER
#define decltype(a,b) decltype(b) // VC++ does not get sane error messages yet I guess -- this ICEs VC++2013
#endif

namespace rikitiki {

     namespace Encoding {
          enum t {
               UNKNOWN,
               chunked,
               compress,
               deflate,
               gzip,
               identity,
               OTHER
          };
          
          t FromString(const wchar_t*);
          const wchar_t* ToString(t);
     }

     std::ostream& operator <<(std::ostream& response, const wchar_t* obj);
     std::ostream& operator <<(std::ostream& response, const std::wstring& obj);

     /**
     Response class that handlers write to. Contains headers, response stream, status, etc.
     */
     struct Response : public IMessage {
     private:
           HeaderCollection headers;
           ByteStream body;
     public:
          Response();
          ~Response();
          uint64_t ContentLength;
          Encoding::t TransferEncoding; 
          
          ContentType::t GetResponseType() const { return ContentType::FromString(ResponseType); }
          void SetResponseType(ContentType::t v) { ResponseType = ContentType::ToString(v); }
          std::wstring ResponseType;  

          virtual HeaderCollection& Headers() OVERRIDE {
               return headers;
          };

          virtual ByteStream& Body() OVERRIDE {
               return body;
          }

          
          const HttpStatus* status;
          void reset();

          std::mutex payloadWrite;

          template <class T> auto operator <<(const T& obj) -> decltype(instance_of<std::stringstream>::value << obj, instance_of<Response&>::value)
          {
               std::lock_guard<std::mutex> lock(payloadWrite);
               Body() << obj;
               assert(Body().good());
               return *this;
          }
          /*const std::stringstream& GetResponse() const;
          std::stringstream& GetResponse();*/
          Response& operator <<(rikitiki::ContentType::t t);
          Response& operator <<(const rikitiki::Cookie& t);
          Response& operator <<(const rikitiki::HttpStatus& t);
          virtual Response& operator <<(const rikitiki::Header& t) OVERRIDE;

     };

     /* Build a response object from a raw string of bytes. */
     class ResponseBuilder {
          enum StateT {
               STATUS,
               HEADERS,
               PAYLOAD,
               CHUNK_PAYLOAD,
               FINISHED
          };
          StateT state;
          enum BufferModeT {
               LENGTH,
               NEWLINE,
               NONE
          };
          BufferModeT bufferMode;
          std::size_t expectedSize;
          std::string buffer; 
          
          bool OnBufferedData(const char*, std::size_t length);
     public:
          Response* response;
          bool OnData(const char*, std::size_t length);
          ResponseBuilder(Response*);          
     };
}

#ifdef _MSC_VER
#undef decltype
#endif