/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#pragma once
#include "../connContext.h"
#include "mongoose.h"

namespace rikitiki {
  namespace mongoose {
    /**
       Connection context for Mongoose servers. 
     */
    class MongooseConnContext : public ConnContext {  
      mg_connection* conn;  

    protected:
      virtual void FillPayload();
      virtual void FillQueryString();
      virtual void FillRequestMethod();
      virtual void FillHeaders();

    public:
      const mg_request_info& request;
      virtual const char* URI();
      MongooseConnContext(const Server* s, 
			  mg_connection* c);
      virtual void writeResponse();
    };

  }
}
