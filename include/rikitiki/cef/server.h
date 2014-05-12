/* Copyright (C) 2012-2013 Justin Berger
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */
#pragma once
#include <rikitiki\server.h>
#include <include\cef_request_handler.h>

namespace rikitiki {
     namespace cef {
          /**
	     Little bit of an odd one. This 'server' is actual intercepting requests from an embedded chromium window. 
             */
       class CefInternalServer : public rikitiki::Server, public CefRequestHandler {
          public:               
	       virtual CefRefPtr<CefResourceHandler> GetResourceHandler(CefRefPtr<CefBrowser> browser,
									CefRefPtr<CefFrame> frame,
									CefRefPtr<CefRequest> request) OVERRIDE; 
              IMPLEMENT_REFCOUNTING(CefInternalServer);
       };
     }
}
