#pragma once 

#include <string>
#include <map>
#include "content_types.h"
#include <vector>
#include <sstream>
#include <mxcomp/useful_macros.h>
#include <mxcomp/cont_stringbuf.h>
#include <assert.h>
#include <rikitiki/http/Enums.h>
#include <rikitiki/http/parsing/BufferedReader.h>
#include <rikitiki/http/parsing/MessageParserState.h>

#pragma warning (disable: 4265)
#include <condition_variable>
#pragma warning (default: 4265)

namespace rikitiki {
     using namespace http::parsing;

     class Cookie;
     class Header;
     class HeaderCollection;
     class CookieCollection;

     // http://www.w3.org/Protocols/rfc2616/rfc2616-sec4.html#sec4
     typedef std::basic_stringstream<char> ByteStream;
     class Message {
     public:          
          virtual ~Message() {}

          virtual HeaderCollection& Headers() = 0;
          virtual ByteStream& Body() = 0;
          virtual CookieCollection& Cookies() = 0;

          virtual size_t ContentLength() const;
          virtual Encoding::t TransferEncoding() const;          
          virtual ContentType::t ContentType() const;
          virtual std::wstring Startline() const = 0; 

          virtual void SetContentLength(size_t);
          virtual void SetTransferEncoding(Encoding::t);
          virtual void SetContentType(ContentType::t t);
          virtual void SetStartline(const std::wstring&) = 0;          

          const HeaderCollection& Headers() const;
          const ByteStream& Body() const;
          const CookieCollection& Cookies() const;

          template <class T> auto operator <<(const T& obj) -> decltype(std::declval<std::stringstream>() << obj, std::declval<Message&>())
          {
               //std::lock_guard<std::mutex> lock(payloadWrite);
               Body() << obj;
               assert(Body().good());
               return *this;
          }
          virtual Message& operator <<(rikitiki::ContentType::t t);
          virtual Message& operator <<(const rikitiki::Cookie& t);
          virtual Message& operator <<(const rikitiki::Header& t);
     };

     std::ostream&  operator<< (std::ostream&,  const Message&);
     std::wostream& operator<< (std::wostream&, const Message&);

}
