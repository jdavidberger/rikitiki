/* Copyright (C) 2012-2013 Justin Berger
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#pragma once
#include <map>
#include <string>
#include <sstream>
#include <rikitiki/http/http_statuses.h>
#include <mxcomp/reflection.h>
#include "content_handler.h"
#include <locale>
#include <codecvt>
#include <rikitiki/http/Request.h>
#include <rikitiki/http/Response.h>

#ifdef _MSC_VER
#undef DELETE
#define decltype(a,b) decltype(b) // VC++ does not get sane error messages yet I guess
#endif
#include <mxcomp\useful_macros.h>

namespace rikitiki {
     class Server;
     class ConnContext;

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

     /***
        Request context object. Contains everything about the request; but has no methods to deal with responding to the request.
        */
     class RequestContext : public IRequest {
     public:
          typedef IRequest::Method Method;
     protected:
          bool mappedQs, mappedHeaders, mappedCookies, mappedPayload;
          QueryStringCollection _qs;
          HeaderCollection _headers;
          CookieCollection _cookies;
          ByteStream _body;
          Method _method;

          virtual void FillQueryString() = 0;
          virtual void FillHeaders() = 0;
          virtual void FillRequestMethod() = 0;
          virtual void FillCookies();

          virtual void FillPayload() = 0;

          RequestContext();
          virtual ~RequestContext();

          /**\brief This is a conv. function to add REQUEST headers, not response headers (use the stream operator for that).
          This function exists so the raw conncontext drivers can just kick down unsanitized header data and this function
          does the right thing. Namely that means lower-casing it.
          */
          HeaderCollection::value_type& AddRequestHeader(const std::wstring&, const std::wstring&);
     public:
          virtual Method& RequestMethod();
          virtual HeaderCollection& Headers();
          virtual CookieCollection& Cookies();
          virtual QueryStringCollection& QueryString();
          virtual const wchar_t* URI() = 0;

          virtual ByteStream& Body() OVERRIDE;

     };

     /**
        Connection Context object. This is the main object in which handlers use both to read request data and
        write response data.

        In general, you can just stream ('<<') whatever you want into it, and there should be an override that
        does the right thing.
        */
     class ConnContext : public virtual RequestContext {
     protected:
          bool mappedPost, mappedContentType;
          bool headersDone;
          PostCollection _post;
          std::multimap<double, ContentType::t>* _accepts;
          ContentType::t _contentType;


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

          virtual void Close();
          bool handled;
          virtual void OnHeadersFinished() {}
          virtual void OnData() {}

          ConnContext& operator <<(const HttpStatus& obj);
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
          virtual size_t rawWrite(const void* buffer, size_t length) = 0;
          virtual void WriteHeaders();           
          virtual void OnHeadersFinished() OVERRIDE;
          virtual void OnData() OVERRIDE;
     public:
          ConnContextWithWrite(Server* s);
          virtual ~ConnContextWithWrite();
          virtual void Close() OVERRIDE;
     };

     void mapContents(ByteStream& raw_content, PostCollection& post);
     void mapQueryString(const wchar_t* _qs, QueryStringCollection& qs);
     ConnContext::Method strToMethod(const wchar_t* method);
     const wchar_t* methodToStr(ConnContext::Method method);
     ConnContext& operator>>(ConnContext&, std::wstring& t);
#include "connContext.tcc"
}

#ifdef _MSC_VER
#undef DELETE
#undef decltype
#endif

//#include "ctemplate/connContext_ext.h"
