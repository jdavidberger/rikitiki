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
          /**
	     Little bit of an odd one. This 'server' is actual intercepting requests from an embedded chromium window. 
             */
		 class CefInternalServer : public rikitiki::websocket::Server, public rikitiki::Server, public CefRequestHandler{
            std::wstring hostname; // We have to pretend to be a host
            virtual websocket::WebsocketProcess* HandleWs(websocket::ConnectionHandle);
           public:               
			   using rikitiki::Server::Register;
			   using rikitiki::websocket::Server::Register;

               CefInternalServer(const std::wstring& _host = L"http://app/");
	       virtual CefRefPtr<CefResourceHandler> GetResourceHandler(CefRefPtr<CefBrowser> browser,
									CefRefPtr<CefFrame> frame,
									CefRefPtr<CefRequest> request) OVERRIDE; 
              

              IMPLEMENT_REFCOUNTING(CefInternalServer);
       };
     }
}
