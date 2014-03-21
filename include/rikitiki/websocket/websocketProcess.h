#pragma once

#include <string>
#include "websocketContext.h"

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

    template <typename P>
      struct WebsocketRoute_ : public WebsocketHandler {
      std::string route;
    WebsocketRoute_(const std::string& r) : route(r) {}
      virtual WebsocketProcess* Handle(WebsocketContext* ctx) {

	bool shouldAttempt =
	  strcmp(route.c_str(), ctx->URI()) == 0;

	if (shouldAttempt){
	  auto rtn = new P(ctx);
	  if (rtn->OnConnect()) {
	    LOG(Web, Verbose) << "Using websocket " << route << std::endl;
	    return rtn;
	  }
	  delete rtn;
	}
	return 0;
      }
    };

  }
}
