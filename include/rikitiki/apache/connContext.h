/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#pragma once
#include <rikitiki/rikitiki.h>
#include <httpd/httpd.h>
#include <httpd/http_protocol.h>
#include <httpd/http_config.h>
#include <httpd/ap_config.h>
#include <httpd/http_core.h>

namespace rikitiki {
  namespace apache {
    /**
       Connection context which deals with apache request_recs
     */
    class ApacheConnContext : public ConnContext {  
    private:      
    protected:
      virtual void FillPayload();
      virtual void FillQueryString();
      virtual void FillRequestMethod();
      virtual void FillHeaders();

    public:      
      request_rec *request;
      virtual const char* URI();
      ApacheConnContext(Server* server, request_rec* r);
      virtual void writeResponse();
    };

  }
}
