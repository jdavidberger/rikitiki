#pragma once
#include <vector>
#include <map>
#include <rikitiki\rikitiki>

namespace rikitiki {
     namespace websocket {
          /*
               There are three active parts to a web socket connection. First, is a server which accepts and delegates incoming and ongoing connections and data. 
               Second is the websocketprocess, which is the actual handler for the various kind of events websockets expose. Third, we have WebsocketContexts, which,
               much like ConnectionContexts for traditional http, the process can manipulate to send data back to the client. 

               We also have handlers for websockets -- they are responsible for seeing an initial websocket request, and generating the correct process for it. 
          */
          struct WebModule;
          class WebsocketProcess;
          class WebsocketContext;
          class WebsocketHandler;
          typedef void* ConnectionHandle; 
          class Server {               
          protected:
               // map of unique ids and open processes. 
               std::map<ConnectionHandle, websocket::WebsocketProcess*> processes;

               // list of handlers, which will ingest a request and generate a process
               std::vector<WebsocketHandler*> wsHandlers;
               WebsocketProcess* HandleWs(WebsocketContext* ctx);
               virtual WebsocketProcess* HandleWs(ConnectionHandle) = 0;

               virtual void OnReady(ConnectionHandle);

               // When the client closes or otherwise loses the connection, this cleans out the process. 
	       virtual void Close(WebsocketContext* ctx);
               // Websocket contexts should be able to close sockets.... but I wonder if this is truely necessary. 
               friend class WebsocketContext;
          public: 
	       virtual void Register(WebModule& t);
               void AddWsHandler(WebsocketHandler* handler);               
               virtual ~Server();
          };
          struct WebModule {
               virtual void Register(rikitiki::websocket::Server&) = 0;
          };

     }
}
