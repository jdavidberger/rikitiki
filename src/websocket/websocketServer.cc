#include <rikitiki/websocket/websocket>

namespace rikitiki {
  namespace websocket {
    void Server::AddWsHandler(WebsocketHandler* handler) {
      wsHandlers.push_back(handler);
    }
    
    WebsocketProcess* Server::HandleWs(WebsocketContext* ctx){
      for(auto it = wsHandlers.begin();it != wsHandlers.end();it++) {
	auto rtn = (*it)->Handle(ctx);
	if(rtn) return rtn;
      }
      return 0;
    }
  }
}
