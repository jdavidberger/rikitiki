/* Copyright (C) 2012-2013 Justin Berger
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#pragma once

#include <type_traits>

#include <mxcomp/useful_macros.h>
#include <mxcomp/log.h>

#include <string>
#include <sstream>

#include <rikitiki/content_handler.h>
#include <rikitiki/http/Request.h>
#include <rikitiki/http/outgoing/Response.h>
#include <rikitiki/exception.h>

namespace rikitiki {
     class Server;
     class MessageListener;

     /** Thrown from within handlers to immediately stop handler execution.
     Note that throwing an exception will treat the request as handled, by design.
     */
     struct HandlerException : public rikitiki::exception {
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
          
          void AddRequestListener(MessageListener*);
	  rikitiki::Request& Request ;
          OResponse& Response ;
     };

     template <class RequestT, class ResponseT, class Data >
     struct Container_ {
          ResponseT Response;
          RequestT  Request;
          Container_(Data& data) : Response(data), Request(data) {}
     };

     template <class RequestT, class ResponseT, class Data = void>
     class ConnContext_ : public Container_<RequestT, ResponseT, Data>, public ConnContext {
     protected:
     ConnContext_(Server* s, Data& data) : Container_<RequestT, ResponseT, Data>(data), ConnContext(s, Request, Response) {}
     public:
          using Container_<RequestT, ResponseT, Data>::Request;
          using Container_<RequestT, ResponseT, Data>::Response;
     };

     void CleanConnContext(ConnContext* ctx);

     template <class T>
     class ConnContextRef_ : public std::shared_ptr<T> {
     public:
          ConnContextRef_() {}
          ConnContextRef_(T*ptr) : std::shared_ptr<T>(ptr, CleanConnContext){
               LOG(Server, Debug) << "Created Context Ref " << (void*)this << std::endl;
          }
     };

     typedef std::shared_ptr<ConnContext> ConnContextRef;


     ConnContext& operator>>(ConnContext&, std::wstring& t);

     void type_conversion_error(ConnContext& ctx, void** handlers);

}

#include "connContext.tcc"
