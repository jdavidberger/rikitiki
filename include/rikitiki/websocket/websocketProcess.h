#pragma once
#include <string>

namespace rikitiki {
     namespace websocket {
          struct WebsocketContext;
          struct Message;
          struct Frame;

          struct WebsocketProcess {
               WebsocketContext* context;
               WebsocketProcess(WebsocketContext*);
               virtual ~WebsocketProcess();

               Message* ongoingMessage;

               virtual bool OnConnect();
               virtual bool OnReady();

               virtual bool OnReceiveFrame(const Frame& frame);
               virtual bool OnReceiveMessage(const Message& message);
               virtual bool OnReceiveText(const char*, size_t);
               virtual bool OnReceiveBinary(const unsigned char*, size_t);
          };

          struct WebsocketHandler {
               virtual WebsocketProcess* Handle(WebsocketContext*) = 0;
          };

          struct WebsocketRoute : public WebsocketHandler {
               std::string route;
               virtual WebsocketProcess* CreateProcessor(WebsocketContext* ctx) = 0;
               WebsocketRoute(const std::string& r);
               virtual WebsocketProcess* Handle(WebsocketContext* ctx);
          };

          template <typename T, typename A>
          struct WebsocketRoute_ : public WebsocketRoute {
               std::string route;
               A arg;
               WebsocketRoute_(const std::string& r, const A& _arg) : WebsocketRoute(r), arg(_arg) {}
               virtual WebsocketProcess* CreateProcessor(WebsocketContext* ctx) {                    
                    return new T(ctx, arg);
               }
          };

          template <typename P>
          struct CreateWsRoute {
               template<typename A>
               static WebsocketRoute* With(A p, const std::string& _route){
                    return new WebsocketRoute_<P, A>(_route, p);
               }
          };
     }

}
