#pragma once

#include <rikitiki\http\Request.h>
#include <rikitiki\http\incoming\Message.h>

namespace rikitiki {     

     struct IRequest : public virtual IMessage {
     
     };

     template <class T> struct IRequest_ : public IMessage_< T >, public IRequest {
          virtual RequestMethod::t RequestMethod();
          virtual const wchar_t* URI();

          virtual CookieCollection& Cookies();
          virtual QueryStringCollection& QueryString();
     };
     
     class SimpleRequest : public Request {
     private:
          RequestMethod::t method = RequestMethod::GET;
          HeaderCollection headers;
          CookieCollection cookies;
          QueryStringCollection queryString;
          
          ByteStream body;

     public:
          std::wstring uri;
          virtual ~SimpleRequest(){}
          virtual RequestMethod::t RequestMethod() const OVERRIDE { 
               return method; 
          };
          void SetRequestMethod(RequestMethod::t r) {
               method = r;
          }
          virtual HeaderCollection& Headers() OVERRIDE {
               return headers;
          };

          virtual CookieCollection& Cookies() OVERRIDE {
               return cookies;
          };

          virtual QueryStringCollection& QueryString() OVERRIDE {
               return queryString;
          };
          
          virtual const wchar_t* URI() const OVERRIDE {
               return uri.data();
          };

          virtual ByteStream& Body() OVERRIDE {
               return body;
          }
     };
}