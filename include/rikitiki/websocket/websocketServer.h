#include "websocketProcess.h"

namespace rikitiki {
  namespace websocket {
    struct Server {
      std::vector<WebsocketHandler*> wsHandlers;
      void AddWsHandler( WebsocketHandler* handler);
      WebsocketProcess* HandleWs(WebsocketContext* ctx);
    };
  }
}
