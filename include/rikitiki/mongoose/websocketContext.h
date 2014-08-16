/* Copyright (C) 2012-2013 Justin Berger
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#pragma once
#include <rikitiki/mongoose/connContext.h>
#include <rikitiki/websocket/websocket>

struct mg_connection;

namespace rikitiki {
     struct Server;

     namespace mongoose {
          /*
          class MongooseWebsocketContext : public MongooseRequestContext,
               public websocket::WebsocketContext {
          private:
               virtual int raw_write(const unsigned char* buffer, size_t len);
          protected:
               void setConnection(mg_connection* conn);
               friend static void _wsReady(struct mg_connection *conn);
          public:
               mg_connection* conn;
               const mg_connection* requestConn;

               MongooseWebsocketContext( websocket::Server* s, const mg_connection* conn);
               MongooseWebsocketContext( websocket::Server* s, mg_connection* conn);
          };
          */
     }
}
