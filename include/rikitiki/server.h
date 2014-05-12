/* Copyright (C) 2012-2013 Justin Berger
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#pragma once 

#include <rikitiki/config.h>
#include <vector>
#include <sstream>
#include "content_types.h"

#include <mxcomp/macros.h>
#include <mxcomp/tuple_ext.h>
#include <tuple>

#include <rikitiki/log/log>
#include <rikitiki/http_statuses.h>
#include <memory>
#ifdef RT_USE_WEBSOCKETS
#include <rikitiki/websocket/websocketServer.h>
#endif

#ifdef RT_USE_CTEMPLATE
#include <rikitiki/ctemplate/templatePreprocessor.h>
#endif

namespace rikitiki {
     class ConnContext;
     class RequestContext;     
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
          
          virtual bool Handle(ConnContextRef ctx) = 0;
          virtual bool CanHandle(RequestContext& ctx) = 0;
          virtual bool visible() const = 0;
          virtual std::wstring name() const = 0;
          virtual std::string desc() const;
          virtual ~Handler();
     };

     struct StaticContentHandler : public Handler {
          std::wstring prefix; 
          std::wstring path; 
          StaticContentHandler(const std::wstring& prefix, const std::wstring& path);
          virtual bool Handle(ConnContextRef ctx) ;
          virtual bool CanHandle(RequestContext& ctx) ;
          virtual bool visible() const ;
          virtual std::wstring name() const ;
          virtual ~StaticContentHandler();
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
#ifdef RT_USE_WEBSOCKETS
          : public rikitiki::websocket::Server
#endif
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
          
          template <typename T>
          void Register(T& t){
               t.Register(*this);
          }
     };

}
