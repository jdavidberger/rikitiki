/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#pragma once
#include "../connContext.h"
#include "mongoose.h"

namespace rikitiki {
  namespace mongoose {

	  class MongooseRequestContext : public virtual RequestContext {
	  protected:
		  virtual void FillQueryString();
	  public:
		  MongooseRequestContext(const mg_request_info*);

		  const mg_request_info& request;
		  virtual const char* URI();
	  };
    /**
       Connection context for Mongoose servers. 
     */
	  class MongooseConnContext : public virtual MongooseRequestContext, public  ConnContext {
      mg_connection* conn;  

    protected:
      virtual void FillPayload();
      
      virtual void FillRequestMethod();
      virtual void FillHeaders();
	  virtual void FillQueryString();
    public:
		
      MongooseConnContext(Server* s, 
			  mg_connection* c);
	  virtual const char* URI();
      virtual void writeResponse();
      virtual int rawWrite(const char* buffer, size_t length);
      virtual void Close();
    };

  }
}
