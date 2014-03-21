#include <rikitiki/websocket/websocket>

namespace rikitiki {
  namespace websocket {

    WebsocketProcess::WebsocketProcess(WebsocketContext* ctx) : context(ctx), ongoingMessage(0) {

    }

    WebsocketProcess::~WebsocketProcess() {};

    bool WebsocketProcess::OnConnect() { return true; }
    bool WebsocketProcess::OnReady() { return true; }
    bool WebsocketProcess::OnReceiveText(const char*, size_t) { return true; }
    bool WebsocketProcess::OnReceiveBinary(const unsigned char*, size_t) { return true; }

    bool WebsocketProcess::OnReceiveMessage(const Message& message) {
		switch (message.opCode) {
		case OpCode::Text:
			return OnReceiveText((const char*)&message.data[0], message.data.size());
		case OpCode::Binary:
			return OnReceiveBinary(&message.data[0], message.data.size());
		} 
		return true;	
    }

    bool WebsocketProcess::OnReceiveFrame(const Frame& frame) {
      if(ongoingMessage == 0) {
	ongoingMessage = new Message(frame);
      } else {
	ongoingMessage->Ingest(frame);
      }

      bool rtn = true;
      if(frame.info.isFinished) {
	rtn = OnReceiveMessage(*ongoingMessage);
	delete ongoingMessage;
	ongoingMessage = 0;      
      }

      return rtn;
    }

  }
}
