/* Copyright (C) 2012-2013 Justin Berger
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#pragma once
#include <rikitiki\connContext.h>
#include <include\cef_request_handler.h>

namespace rikitiki {
     struct Server;

     namespace cef {

       class RequestContext : public virtual rikitiki::RequestContext {
          protected:
               virtual void FillQueryString(){}
               virtual void FillHeaders(){}
               virtual void FillRequestMethod(){}
	       CefRefPtr<CefRequest> request;
               std::wstring url; 
          public:
               RequestContext(CefRefPtr<CefRequest> req) : request(req) {}
	       virtual const wchar_t* URI() OVERRIDE;
          };

     }
}
