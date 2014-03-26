#pragma once

#include <string>
#include "websocketContext.h"
#include <mxcomp\log.h>

namespace rikitiki {
  namespace websocket {
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
		WebsocketRoute(const std::string& r) : route(r) {}
		
		virtual WebsocketProcess* Handle(WebsocketContext* ctx) {

			bool shouldAttempt =
				strcmp(route.c_str(), ctx->URI()) == 0;

			if (shouldAttempt){
				auto rtn = CreateProcessor(ctx);
				if (rtn->OnConnect()) {
					LOG(Web, Verbose) << "Using websocket " << route << std::endl;
					return rtn;
				}
				delete rtn;
			}
			return 0;
		}
	};
	
    template <typename T>
	struct WebsocketRoute_ : public WebsocketRoute {
      std::string route;
	  
	  WebsocketRoute_(const std::string& r) : WebsocketRoute(r) {}
	  virtual WebsocketProcess* CreateProcessor(WebsocketContext* ctx) {
		  return new T(ctx);
	  }
    };

  }
}
