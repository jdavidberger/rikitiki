/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#pragma once
#include "../connContext.h"
#include <mongoose.h>

namespace rikitiki {
  namespace mongoose {

    class MongooseConnContext : public ConnContext {  
      mg_event event;
      mg_connection* conn;  

    protected:
      virtual void FillPost();
      virtual void FillQueryString();
      virtual void FillRequestMethod();
      virtual void FillHeaders();

    public:
      const mg_request_info& request;
      virtual const char* URI();
      MongooseConnContext(const Server* s, 
			  mg_event e, 
			  mg_connection* c);
      virtual void writeResponse();
    };

  }
}
