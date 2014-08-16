#pragma once
#include <mxcomp/useful_macros.h>
#include <rikitiki/http/outgoing/Message.h>
#include <rikitiki/http/Response.h>

namespace rikitiki {

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
