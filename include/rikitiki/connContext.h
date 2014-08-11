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
#include <rikitiki/http/incoming/Request.h>
#include <rikitiki/http/outgoing/Response.h>


#include <rikitiki/requestContext.h>

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

     /**
        Connection Context object. This is the main object in which handlers use both to read request data and
        write response data.

        In general, you can just stream ('<<') whatever you want into it, and there should be an override that
        does the right thing.
        */    
     class ConnContext {
     protected:
          friend class Server;

          ConnContext(Server*, Request&, OResponse&);
          
     public:
          ConnContext& operator=(const ConnContext&) = delete;
          virtual ~ConnContext();
          Server* server;
          
          virtual void Close();
          bool handled;

          ConnContext& operator <<(const HttpStatus& obj);
          ConnContext& operator <<(const Cookie& obj);
          ConnContext& operator <<(const Header& obj);
          ConnContext& operator <<(std::function<void(std::wostream&)>);
          template <class T> ConnContext& operator <<(const T& obj);

          template <class T>
          typename std::enable_if<  std::is_class<typename valid_conversions<T>::In>::value,
               ConnContext&>::type operator <<(T&);
          
          template <class T> auto operator >>(T&) -> decltype(valid_conversions<T>::In::Instance(), instance_of<ConnContext>::value);

          /*
          HeaderCollection& Headers() {
               return request.Headers();
          };
          ByteStream& Body() {
               return request.Body();
          };
          CookieCollection& Cookies() {
               return request.Cookies();
          }
          QueryStringCollection& QueryString() {
               return request.QueryString();
          }
          */
          DEPRECATED("Access request method via request") RequestMethod::t RequestMethod() const {
               return Request.RequestMethod();
          };
          DEPRECATED("Access uri via request") const wchar_t* URI() const {
               return Request.URI();
          }

          void AddRequestListener(MessageListener*);
          Request& Request ;
          OResponse& Response ;
     };

     template <class RequestT, class ResponseT, class Data >
     struct Container_ {
          ResponseT Response;
          RequestT  Request;
          Container_(const Data& data) : Response(data), Request(data) {}
     };

     template <class RequestT, class ResponseT, class Data = void>
     class ConnContext_ : public Container_<RequestT, ResponseT, Data>, public ConnContext {
     protected:
          ConnContext_(Server* s, const Data& data) : Container_(data), ConnContext(s, Request, Response) {}
     public:
          using Container_<RequestT, ResponseT, Data>::Request;
          using Container_<RequestT, ResponseT, Data>::Response;
     };

     /*
     class ConnContextWithWrite : public ConnContext {
     private:

     protected:
          virtual size_t rawWrite(const void* buffer, size_t length) = 0;
          virtual void WriteHeaders();           
          virtual void OnHeadersFinished() OVERRIDE;
          virtual void OnData() OVERRIDE;
     public:
          ConnContextWithWrite(Server* s, Request& r);
          virtual ~ConnContextWithWrite();
          virtual void Close() OVERRIDE;
     };
     */
     //void mapContents(ByteStream& raw_content, PostCollection& post);
     //void mapQueryString(const wchar_t* _qs, QueryStringCollection& qs);
     ConnContext& operator>>(ConnContext&, std::wstring& t);
}

#include "connContext.tcc"
