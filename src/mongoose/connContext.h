#pragma once
#include "../connContext.h"

class MongooseConnContext : public ConnContext {  
  mg_event event;
  mg_connection* conn;  

 protected:
  virtual void FillPost();
  virtual void FillRequestMethod();
 public:
  const mg_request_info& request;
  virtual const char* URI();
  MongooseConnContext(const Server* s, 
		       mg_event e, 
		       mg_connection* c);
  virtual void writeResponse();
};

