/* Copyright (C) 2012-2013 Justin Berger
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */
#pragma once
#include <rikitiki\server.h>

#define USING_CEF_SHARED 1
#pragma warning(disable: 4100 4668)
#include <include/cef_request_handler.h>
#include <include/cef_scheme.h>
#include <include/cef_client.h>
#include <include/cef_app.h>
#pragma warning(default:  4100 4668)

#include <rikitiki\websocket\websocket>

namespace rikitiki {
     namespace cef {
#ifdef RT_USE_WEBSOCKET
          class CefInternalWebsocket : public rikitiki::websocket::Server {
          protected:
               virtual websocket::WebsocketProcess* HandleWs(websocket::ConnectionHandle);
          public:
               using rikitiki::websocket::Server::Register;
          };
#endif
          /**
             Little bit of an odd one. This 'server' is actual intercepting requests from an embedded chromium window.
             */
          class CefInternalServer :
#ifdef RT_USE_WEBSOCKET                      
               public CefInternalWebsocket, 
#endif
               public rikitiki::Server,
               public CefRequestHandler,
               public CefSchemeHandlerFactory {

               // We have to pretend to be a host
               std::wstring hostname; 

          public:
               using rikitiki::Server::Register;
               CefInternalServer(const std::wstring& _host = L"http://app");
               
               virtual CefRefPtr<CefResourceHandler> GetResourceHandler(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request) OVERRIDE;

               virtual std::future<std::shared_ptr<rikitiki::Response>> ProcessRequest(rikitiki::Request&) OVERRIDE;

               virtual CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser>, CefRefPtr<CefFrame>, const CefString&, CefRefPtr<CefRequest>) OVERRIDE;

               IMPLEMENT_REFCOUNTING(CefInternalServer);
          };
     }
}
