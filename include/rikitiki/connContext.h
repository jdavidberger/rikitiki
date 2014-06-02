/* Copyright (C) 2012-2013 Justin Berger
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#pragma once
#include <map>
#include <string>
#include <sstream>
#include "http_statuses.h"
#include <mxcomp/reflection.h>
#include "content_handler.h"
#include <locale>
#include <codecvt>
#ifdef _MSC_VER
#undef DELETE
#define decltype(a,b) decltype(b) // VC++ does not get sane error messages yet I guess
#endif
#include <mxcomp\useful_macros.h>

namespace rikitiki {
     class Server;
     class ConnContext;


     std::ostream& operator <<(std::ostream& response, const wchar_t* obj);
     std::ostream& operator <<(std::ostream& response, const std::wstring& obj);

     /** Thrown from within handlers to immediately stop handler execution.
     Note that throwing an exception will treat the request as handled, by design.
     */
     struct HandlerException {
          HandlerException() : status(0){}
          HandlerException(const HttpStatus& s) : status(&s){}
          /** Optionally specify the status to return.
          If no status is set, Internal_Server_Error is returned.
          */
          const HttpStatus* status;
     };

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
        Response class that handlers write to. Contains headers, response stream, status, etc.
        */
     struct Response {
          ContentType::t GetResponseType() const { return ContentType::FromString(ResponseType); }
          void SetResponseType(ContentType::t v) { ResponseType = ContentType::ToString(v);  }
          std::wstring ResponseType;
          std::vector<Header> headers;
          const HttpStatus* status;
          std::stringstream response;
          void reset();
          Response();

          template <class T>
          auto operator <<(const T& obj) -> decltype(instance_of<std::stringstream>::value << obj, instance_of<Response&>::value)
          {
               response << obj; return *this;
          }

          Response& operator <<(rikitiki::ContentType::t t);
          Response& operator <<(const rikitiki::Cookie& t);
          Response& operator <<(const rikitiki::HttpStatus& t);
          Response& operator <<(const rikitiki::Header& t);
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

     /***
        Request context object. Contains everything about the request; but has no methods to deal with responding to the request.
        */
     class RequestContext {     
     public:
          enum Method {
               ANY = 0, GET = 1, POST = 2, HEAD = 3, PUT = 4, DELETE = 5, TRACE = 6, OPTIONS = 7, CONNECT = 8, PATCH = 9, OTHER
          };
     protected:
          bool mappedQs, mappedHeaders, mappedCookies;
          QueryStringCollection _qs;
          HeaderCollection _headers;
          CookieCollection _cookies;

          Method _method;

          virtual void FillQueryString() = 0;
          virtual void FillHeaders() = 0;
          virtual void FillRequestMethod() = 0;
          virtual void FillCookies();
          
          RequestContext();
          virtual ~RequestContext();

          /**\brief This is a conv. function to add REQUEST headers, not response headers (use the stream operator for that).
          This function exists so the raw conncontext drivers can just kick down unsanitized header data and this function
          does the right thing. Namely that means lower-casing it.
          */
          HeaderCollection::value_type& AddRequestHeader(const std::wstring&, const std::wstring&);          
     public:
          Method RequestMethod();
          HeaderCollection& Headers();
          CookieCollection& Cookies();
          QueryStringCollection& QueryString();
          virtual const wchar_t* URI() = 0;
     };

     /**
        Connection Context object. This is the main object in which handlers use both to read request data and
        write response data.

        In general, you can just stream ('<<') whatever you want into it, and there should be an override that
        does the right thing.
        */
     class ConnContext : public virtual RequestContext {
     protected:
          bool mappedPost, mappedPayload, mappedContentType;
          bool headersDone;
          PostCollection _post;
          std::multimap<double, ContentType::t>* _accepts;
          ContentType::t _contentType;
          std::wstring _payload;

          virtual void FillPayload() = 0;
          virtual void FillAccepts();
          virtual void FillContentType();          
          virtual void FillPost();
          
          friend class Server;


          ConnContext(Server*);
          ConnContext();
          ConnContext(ConnContext&) = delete;
          ConnContext& operator=(const ConnContext& rhs) = delete;

     public:
          virtual ~ConnContext();
          Server* server;
          std::multimap<double, ContentType::t>& Accepts();
          PostCollection& Post();
          ContentType::t ContentType();

          std::wstring& Payload();
          
          virtual void Close() = 0;
          bool handled;
          virtual void OnHeadersFinished() {}
          virtual void OnData() {}

          ConnContext& operator <<(const Cookie& obj);
          ConnContext& operator <<(const Header& obj);

          ConnContext& operator <<(std::function<void(std::wostream&)>);
          template <class T> ConnContext& operator <<(const T& obj);

          template <class T>
          typename std::enable_if<  std::is_class<typename valid_conversions<T>::In>::value,
               ConnContext&>::type operator <<(T&);
          template <class T> auto operator >>(T&) -> decltype(valid_conversions<T>::In::Instance(), instance_of<ConnContext>::value);

          Response response;
     };
     
     class ConnContextWithWrite : public ConnContext {
     private:
          
     protected:
          virtual void writeResponse();
		  virtual size_t rawWrite(const void* buffer, size_t length) = 0;

     public:
          virtual void Close();

          ConnContextWithWrite(Server* s);
          virtual ~ConnContextWithWrite();
     };

     void mapContents(std::wstring& raw_content, PostCollection& post);
     void mapQueryString(const wchar_t* _qs, QueryStringCollection& qs);
     ConnContext::Method strToMethod(const wchar_t* method);

     ConnContext& operator>>(ConnContext&, std::wstring& t);
#include "connContext.tcc"
}

#ifdef _MSC_VER
#undef DELETE
#undef decltype
#endif

//#include "ctemplate/connContext_ext.h"
