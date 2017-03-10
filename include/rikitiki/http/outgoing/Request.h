#pragma once

#include <rikitiki/http/Request.h>
#include <rikitiki/http/outgoing/Message.h>

namespace rikitiki {     

     struct ORequest : public virtual OMessage {
          
     };

     class ORequestMemory : public ORequest, public virtual Request {
     private:
          RequestMethod::t method = RequestMethod::GET;
          HeaderCollection headers;
          CookieCollection cookies;
          QueryStringCollection queryString;

          ByteStream body;
         QueryStringCollection post;
     public:
          
          rikitiki::string uri;

          virtual size_t WritePayloadData(const char*, size_t) OVERRIDE;
          virtual void WriteHeader(const Header&) OVERRIDE;

         QueryStringCollection &Post() override;

         virtual ~ORequestMemory(){}
          virtual RequestMethod::t RequestMethod() const OVERRIDE{
               return method;
          };
          void SetRequestMethod(RequestMethod::t r) {
               method = r;
          }
          virtual HeaderCollection& Headers() OVERRIDE{
               return headers;
          };

          virtual CookieCollection& Cookies() OVERRIDE{
               return cookies;
          };

          virtual QueryStringCollection& QueryString() OVERRIDE{
               return queryString;
          };

          virtual const rikitiki::string::value_type * URI() const OVERRIDE{
               return uri.data();
          };

          virtual ByteStream& Body() OVERRIDE{
               return body;
          }

         size_t WriteData(const char *string1, size_t size) override;
     };

     typedef ORequestMemory SimpleRequest;
}
