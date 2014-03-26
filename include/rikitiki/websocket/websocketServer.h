#pragma once
#include <vector>

namespace rikitiki {
  namespace websocket {

	  class WebsocketProcess;
	  class WebsocketHandler;
	  class WebsocketContext;

    struct Server {
      std::vector<WebsocketHandler*> wsHandlers;
      void AddWsHandler( WebsocketHandler* handler);
      WebsocketProcess* HandleWs(WebsocketContext* ctx);
    };
  }
}
