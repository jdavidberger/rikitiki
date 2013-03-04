/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include "server.h"
#include <sstream>
#include "connContext.h"

namespace rikitiki {
  namespace mongoose {

    MongooseServer::MongooseServer(int _port) : port(_port)  { 
      ctx = 0;
    }

    static void* _handler(enum mg_event event, struct mg_connection *conn) {
      const struct mg_request_info *request_info = mg_get_request_info(conn);

      MongooseServer* server = ((MongooseServer*)(request_info->user_data));

      if(event != MG_NEW_REQUEST)
	return (void*)0;
      
      mongoose::MongooseConnContext ctx(server, event, conn);
      return server->Handle(ctx) ? (void*)1 : (void*)0;
    }

    void MongooseServer::Start() {  
      std::stringstream _port;
      _port << port;
      std::string __port = _port.str();
      const char *options[] = {"listening_ports", __port.c_str(), NULL};
      ctx = mg_start(_handler, this, options);
    }

    void MongooseServer::Stop() {
      mg_stop(ctx);
      ctx = 0;
    }
  }
}
