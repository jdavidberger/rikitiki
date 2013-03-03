#pragma once
#include "../connContext.h"
#include <httpd/httpd.h>
#include <httpd/http_protocol.h>
#include <httpd/http_config.h>
#include <httpd/ap_config.h>
#include <httpd/http_core.h>

class ApacheConnContext : public ConnContext {  
 protected:
  virtual void FillPost();
  virtual void FillRequestMethod();
 public:
  request_rec *request;
  virtual const char* URI();
  ApacheConnContext(request_rec* r);
  virtual void writeResponse();
};

