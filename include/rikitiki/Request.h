#pragma once

#include <string>
#include <map>
#include <sstream>

#ifdef _MSC_VER
#undef DELETE
#endif

namespace rikitiki {
     typedef std::pair<std::wstring, std::wstring> wstringpair;

     /**
     Headers are just string pairs
     We can't just typedef it since we want to pass it around with stream operators
     */
     struct Header : public wstringpair {
          Header(const std::wstring& name, const std::wstring& value);
     };

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


     /**
     The normal multimap, but with an operator[] defined.
     */
     template <typename T1, typename T2>
     struct multimap : public std::multimap<T1, T2> {
          T2& operator[](const T1&);
          template <typename S>
          bool hasValue(const T1& key, S& rtn) {
               typename rikitiki::multimap<T1, T2>::iterator it = this->find(key);
               if (it != this->end()){
                    rtn = it->second;
                    return true;
               }
               return false;
          }
     };

     typedef multimap<std::wstring, std::wstring> HeaderCollection;
     typedef std::map<std::wstring, std::wstring> QueryStringCollection;
     typedef multimap<std::wstring, std::wstring> PostCollection;
     typedef std::map<std::wstring, std::wstring> CookieCollection;

     //typedef std::basic_string<char> ByteStream;
     typedef std::basic_stringstream<char> ByteStream;
     class IRequest {
     public:
          enum Method {
               ANY = 0, GET = 1, POST = 2, HEAD = 3, PUT = 4, DELETE = 5, TRACE = 6, OPTIONS = 7, CONNECT = 8, PATCH = 9, OTHER
          };
          virtual Method RequestMethod() = 0;
          virtual HeaderCollection& Headers() = 0;
          virtual CookieCollection& Cookies() = 0;
          virtual QueryStringCollection& QueryString() = 0;
          virtual const wchar_t* URI() = 0;
          virtual const ByteStream& Payload() = 0;
     };

     class SimpleRequest : public IRequest {
     public:
          virtual ~SimpleRequest(){}
          Method method;
          virtual Method RequestMethod() { return method; };
          HeaderCollection headers;
          virtual HeaderCollection& Headers() {
               return headers;
          };

          CookieCollection cookies;
          virtual CookieCollection& Cookies() {
               return cookies;
          };

          QueryStringCollection queryString;
          virtual QueryStringCollection& QueryString() {
               return queryString;
          };
          std::wstring uri;
          virtual const wchar_t* URI() {
               return uri.data();
          };

          ByteStream payload;

          ByteStream& Payload() {
               return payload;
          }
     };
}