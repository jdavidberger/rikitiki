/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include <rikitiki/mongoose/websocketContext.h>
#include <cstring>


namespace rikitiki {
  namespace mongoose {
	  const char* MongooseWebsocketContext::URI() {
		  mg_connection* c = conn;
		  if (c == 0) {
			  c = const_cast<mg_connection*>(requestConn);
		  }

		  auto request = mg_get_request_info(c);
		  return request->uri;
	  }

	  void MongooseWebsocketContext::FillQueryString() {
		  MongooseRequestContext::FillQueryString();
	  }
	MongooseWebsocketContext::MongooseWebsocketContext(const Server* s, const mg_connection* conn) : requestConn(conn), conn(0), MongooseRequestContext(mg_get_request_info(const_cast<mg_connection*>(conn))) {
		
    }
	  MongooseWebsocketContext::MongooseWebsocketContext(const Server* s, mg_connection* conn) : requestConn(conn), conn(conn), MongooseRequestContext(mg_get_request_info(conn)) {
      
    }
    int MongooseWebsocketContext::raw_write(const unsigned char* buffer, size_t len) {
      return mg_write(conn, buffer, len);  
    }
  }
}
