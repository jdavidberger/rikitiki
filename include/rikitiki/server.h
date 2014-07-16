/* Copyright (C) 2012-2013 Justin Berger
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#pragma once 

#include <rikitiki/config.h>
#include <vector>
#include <sstream>
#include <rikitiki/http/content_types.h>

#include <mxcomp/macros.h>
#include <mxcomp/tuple_ext.h>
#include <mxcomp/useful_macros.h>
#include <tuple>

#include <rikitiki/log/log>
#include <rikitiki/http/http_statuses.h>
#include <memory>
#pragma warning(disable:4265 4355 4062)
#include <future>
#pragma warning(default:4265 4355 4062)
#ifdef RT_USE_CTEMPLATE
#include <rikitiki/ctemplate/templatePreprocessor.h>
#endif

namespace rikitiki {
     class ConnContext;
     class IRequest;
     class RequestContext;
     class Socket;
     class Response;
     void CleanConnContext(ConnContext* ctx);

     template <class T>
     class ConnContextRef_ : public std::shared_ptr<T> {
     public:
          ConnContextRef_() {}
          ConnContextRef_(T*ptr) : std::shared_ptr<T>(ptr, CleanConnContext){}
     };

     typedef std::shared_ptr<ConnContext> ConnContextRef;

     template <class T>
     std::shared_ptr<ConnContext>& operator<<(std::shared_ptr<ConnContext>& me, T& t)
     {
          *me << t;
          return me;
     }

     template <class T>
     std::shared_ptr<ConnContext>& operator<<(std::shared_ptr<ConnContext>& me, const T& t)
     {
          *me << t;
          return me;
     }

     /**
        Base handler class. These are checked in order whenever there is a request.
        */
     struct Handler {

          virtual bool Handle(ConnContextRef ctx);
          virtual bool CanHandle(RequestContext& ctx) = 0;
          virtual bool visible() const = 0;
          virtual std::wstring name() const = 0;
          virtual std::string desc() const;
          virtual ~Handler();
     };

     class Server;

     struct WebModule {
          virtual void Register(rikitiki::Server&) = 0;
     };


     /**\brief The main interface file between modules and a given server
        Server is the rikitiki component that stores handlers and interfaces with the
        choosen server module to fire them off. It also is the object that provides
        information about the current server -- IP, Port, etc.
        */
     class Server
     {
     protected:
     public:
          std::vector<Handler*> handlers;
          virtual ~Server();
          Server();

#ifdef RT_USE_CTEMPLATE
          std::vector<ctemplates::TemplatePreprocessor*> templatePreprocessors;
          void AddPreprocessor( rikitiki::ctemplates::TemplatePreprocessor*);
#endif
          bool Handle(ConnContextRef ctx);
          Handler* GetHandler(RequestContext& ctx);

          void AddHandler(Handler& handler);
          void AddHandler(Handler* handler);

          /*
          Provides an implementation specific socket which goes through the server. In most cases
          this will just open up a TCP/IP socket and return it.

          This is mostly useful for testing -- it allows one test suite to hit every server implementation.
          */
          //virtual std::auto_ptr<Socket> GetDirectSocket();
          virtual std::future<std::shared_ptr<Response>> ProcessRequest(IRequest&) = 0;

          virtual void Register(WebModule& t);
     };

}
