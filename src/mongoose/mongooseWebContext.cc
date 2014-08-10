/* Copyright (C) 2012-2013 Justin Berger
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include <rikitiki/mongoose/websocketContext.h>

namespace rikitiki {
     namespace mongoose {
          /*
          void MongooseWebsocketContext::setConnection(mg_connection* conn) {
               this->conn = conn;
               this->request = mg_get_request_info(conn);
          }
          MongooseWebsocketContext::MongooseWebsocketContext(websocket::Server* server, const mg_connection* conn) :
               MongooseRequestContext(mg_get_request_info(const_cast<mg_connection*>(conn))),
               requestConn(conn), conn(0), WebsocketContext(server) {

          }
          MongooseWebsocketContext::MongooseWebsocketContext(websocket::Server* s, mg_connection* conn) : requestConn(conn), conn(conn), MongooseRequestContext(mg_get_request_info(conn)), WebsocketContext(s)  {

          }
          int MongooseWebsocketContext::raw_write(const unsigned char* buffer, size_t len) {
               if (conn == 0)
                    throw std::exception("Invalid socket write; tried to write to a socket before a connection was fully established.");
               return mg_write(conn, buffer, len);
          }
          */
     }
     
}
