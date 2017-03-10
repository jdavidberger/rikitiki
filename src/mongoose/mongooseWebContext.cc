/* Copyright (C) 2012-2013 Justin Berger
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include <rikitiki/mongoose/websocketContext.h>
#include <mongoose.h>

namespace rikitiki {
     namespace mongoose {
         #ifdef RT_USE_MONGOOSE
          void MongooseWebsocketContext::setConnection(mg_connection* conn) {
               this->conn = conn;
          }
          MongooseWebsocketContext::MongooseWebsocketContext(websocket::Server* server, const mg_connection* conn) :
               requestConn(conn), conn(0), WebsocketContext(server) {

          }
          int MongooseWebsocketContext::raw_write(const unsigned char* buffer, size_t len) {
               if (conn == 0)
                    throw std::runtime_error("Invalid socket write; tried to write to a socket before a connection was fully established.");
               mg_send(conn, buffer, len);
               return len;
          }

         const char *MongooseWebsocketContext::URI() const {
             return "";
         }


         #endif
     }
     
}
