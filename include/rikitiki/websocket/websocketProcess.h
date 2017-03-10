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
	       virtual void OnClose();
               virtual bool OnReceiveFrame(const Frame& frame);
               virtual bool OnReceiveMessage(const Message& message);
               virtual bool OnReceiveText(const char*, size_t);
               virtual bool OnReceiveBinary(const unsigned char*, size_t);
          };

          struct WebsocketHandler {
               virtual WebsocketProcess* Handle(WebsocketContext*) = 0;
          };

          struct WebsocketRoute : public WebsocketHandler {
               rikitiki::string route;
               virtual ~WebsocketRoute(){}
               virtual WebsocketProcess* CreateProcessor(WebsocketContext* ctx) = 0;
               WebsocketRoute(const rikitiki::string& r);
               virtual WebsocketProcess* Handle(WebsocketContext* ctx);
          };

          template <typename T, typename... A> T* create(WebsocketContext* ctx, const A&... args) {
               
          }

          template <typename T, typename... A>
          struct WebsocketRoute_ : public WebsocketRoute {
               rikitiki::string route;
               std::tuple<A...> args; 
               WebsocketRoute_(const rikitiki::string& r, A... _arg) : WebsocketRoute(r), args(_arg...) {

               }
               T* create(WebsocketContext* ctx, const A&... args) {
                    return new T(ctx, args...);
               }
               virtual WebsocketProcess* CreateProcessor(WebsocketContext* ctx) {
                    std::tuple<WebsocketContext*, A...> applyArgs = std::tuple_cat(std::make_tuple(ctx), args);
                    return mxcomp::tuples::applyTuple(this, &WebsocketRoute_<T,A...>::create, applyArgs);
               }
          };

          template <typename P>
          struct CreateWsRoute {
               template<typename... A>
               static WebsocketRoute* With(const rikitiki::string& _route, A... p){
                    return new WebsocketRoute_<P, A...>(_route, p...);
               }
          };
     }

}
