#pragma once

#include <string>
#include <map>
#include <sstream>
#include <mxcomp\useful_macros.h>
#include "Message.h"

#ifdef _MSC_VER
#undef DELETE
#endif

namespace rikitiki {
     /**
     Things parsed out of the response, ala forms
     We can't just typedef it since we want to pass it around with stream operators
     */
     struct PostContent : public wstringpair {
          PostContent(const std::wstring& name, const std::wstring& value);
     };

     /**
     Cookies are key value pairs too.
     TODO: Add expiration, domain, etc
     */
     struct Cookie : public wstringpair {
          Cookie(const std::wstring& name, const std::wstring& value,
               const std::wstring& Domain = L"", const std::wstring& Path = L"/",
               const std::wstring& Expires = L"", bool secure = false, bool httpOnly = false);
     };

     typedef std::map<std::wstring, std::wstring> QueryStringCollection;
     typedef multimap<std::wstring, std::wstring> PostCollection;
     typedef std::map<std::wstring, std::wstring> CookieCollection;

     // http://www.w3.org/Protocols/rfc2616/rfc2616-sec5.html
     class IRequest : public IMessage {
     public:
          enum Method {
               ANY = 0, GET = 1, POST = 2, HEAD = 3, PUT = 4, DELETE = 5, TRACE = 6, OPTIONS = 7, CONNECT = 8, PATCH = 9, OTHER
          };
          virtual Method& RequestMethod() = 0;
          virtual const wchar_t* URI() = 0;

          virtual CookieCollection& Cookies() = 0;
          virtual QueryStringCollection& QueryString() = 0;          
     };

     class SimpleRequest : public IRequest {
     private:
          Method method = GET;
          HeaderCollection headers;
          CookieCollection cookies;
          QueryStringCollection queryString;
          
          ByteStream body;

     public:
          std::wstring uri;
          virtual ~SimpleRequest(){}
          virtual Method& RequestMethod() { return method; };
     
          virtual HeaderCollection& Headers() OVERRIDE {
               return headers;
          };

          virtual CookieCollection& Cookies() OVERRIDE {
               return cookies;
          };

          virtual QueryStringCollection& QueryString() OVERRIDE {
               return queryString;
          };
          
          virtual const wchar_t* URI() OVERRIDE {
               return uri.data();
          };

          virtual ByteStream& Body() OVERRIDE {
               return body;
          }
     };
}