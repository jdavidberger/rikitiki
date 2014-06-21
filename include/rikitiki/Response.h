#pragma once

#include <string>
#include <rikitiki\Request.h>
#include <sstream>
#include <rikitiki\content_types.h>
#include <vector>
#include <rikitiki\http_statuses.h>
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
     struct Response {
     private:

     public:
          Response();
          ~Response();

          ByteStream payload;
          uint64_t ContentLength;
          Encoding::t TransferEncoding; 
          
          ContentType::t GetResponseType() const { return ContentType::FromString(ResponseType); }
          void SetResponseType(ContentType::t v) { ResponseType = ContentType::ToString(v); }
          std::wstring ResponseType;          
          std::vector<Header> headers;

          const HttpStatus* status;
          void reset();

          std::mutex payloadWrite;

          template <class T> auto operator <<(const T& obj) -> decltype(instance_of<std::stringstream>::value << obj, instance_of<Response&>::value)
          {
               std::lock_guard<std::mutex> lock(payloadWrite);
               payload << obj;
               assert(payload.good());
               return *this;
          }
          /*const std::stringstream& GetResponse() const;
          std::stringstream& GetResponse();*/
          Response& operator <<(rikitiki::ContentType::t t);
          Response& operator <<(const rikitiki::Cookie& t);
          Response& operator <<(const rikitiki::HttpStatus& t);
          Response& operator <<(const rikitiki::Header& t);
     };

     /* Build a response object from a raw string of bytes. */
     struct ResponseBuilder {
          enum StateT {
               STATUS,
               HEADERS,
               PAYLOAD,
               FINISHED
          };
          StateT state;
          std::size_t expectedSize;
          bool OnData(const char*, std::size_t length);
          Response* response;
          ResponseBuilder(Response*);          
     };
}

#ifdef _MSC_VER
#undef decltype
#endif