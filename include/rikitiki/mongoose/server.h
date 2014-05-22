/* Copyright (C) 2012-2013 Justin Berger
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */
#pragma once
#include <rikitiki\server.h>
#include <mongoose.h>
#if RT_USE_WEBSOCKET
#include <rikitiki\websocket\websocketContext.h>
#endif


namespace rikitiki {
     
     namespace mongoose {
          /**
             Mongoose wrapper. Contains base server functionality, plus also exposes Start/Stop methods.
             */
          class MongooseServer : public rikitiki::Server {
               mg_context *ctx;
               std::vector<const char*> options;
               int port;
#ifdef RT_USE_WEBSOCKET
          private:
               static int _wsHandler(const struct mg_connection *conn);
               static void _wsReady(struct mg_connection *conn);
               static int _wsReceive(struct mg_connection *conn, int bits, char* data, size_t length);
          protected:
               virtual void Close(websocket::WebsocketContext*);
               virtual websocket::WebsocketProcess* HandleWs(websocket::ConnectionHandle);
#endif 
          public:                              
               std::string DocumentRoot;
               int Port();
               MongooseServer(int _port);

               /** Blocking call to start the mongoose server. Sets up an interrupt handler to stop the server with siginterrupt.
                   If you have more than one server running, SIGINT shuts them all down.
                   */
               void Run();
               /** Non-blocking call to start the mongoose server.*/
               void Start();
               /** Stop a running server */
               void Stop();
          };
     }
}
