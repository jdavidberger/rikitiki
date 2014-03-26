/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#pragma once
#include "../connContext.h"
#include "mongoose.h"
#include "../websocket/websocket"
#include "connContext.h"
namespace rikitiki {
  namespace mongoose {

	  class MongooseWebsocketContext : public MongooseRequestContext, public websocket::WebsocketContext {
      
      virtual int raw_write(const unsigned char* buffer, size_t len);      
	  protected:
		  virtual void FillQueryString();
    public:
      mg_connection* conn;
      const mg_connection* requestConn;
      virtual const char* URI();      
      MongooseWebsocketContext(const Server* s, const mg_connection* conn);
      MongooseWebsocketContext(const Server* s, mg_connection* conn);
    };

  }
}
