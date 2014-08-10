#pragma once

#include <string>
#include <map>
#include <sstream>
#include <mxcomp\useful_macros.h>
#include "Message.h"
#include "Enums.h"
#include <rikitiki\http\Header.h>

namespace rikitiki {     

     // http://www.w3.org/Protocols/rfc2616/rfc2616-sec5.html
     class Request : public virtual Message {
     public:          
          typedef RequestMethod::t Method;
          virtual RequestMethod::t RequestMethod() const = 0;
          virtual void SetRequestMethod(RequestMethod::t) = 0;

          virtual const wchar_t* URI() const = 0;

          virtual QueryStringCollection& QueryString() = 0;          

          virtual void SetStartline(const std::wstring&) OVERRIDE;
          virtual std::wstring Startline() const OVERRIDE;
     };
     
     struct IRequest : public virtual IMessage, public virtual Request {
     
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