/* Copyright (C) 2012-2013 Justin Berger
The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#pragma once
#include <rikitiki/http/Request.h>

namespace rikitiki {
     class Server;
     class ConnContext;

     /***
     Request context object. Contains everything about the request; but has no methods to deal with responding to the request.

     Seperated out for protocols which are initiated via a HTTP request but not followed up by an HTTP response (websockets)
     */
     class RequestContext : public virtual IRequest {
     public:
          
     protected:
          RequestContext();
          virtual ~RequestContext();

          /**\brief This is a conv. function to add REQUEST headers, not response headers (use the stream operator for that).
          This function exists so the raw conncontext drivers can just kick down unsanitized header data and this function
          does the right thing. Namely that means lower-casing it.
          */
          HeaderCollection::value_type& AddRequestHeader(const std::wstring&, const std::wstring&);
     public:
          
     };

}

