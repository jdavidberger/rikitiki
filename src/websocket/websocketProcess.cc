#include <rikitiki/websocket/websocket>
#include <mxcomp/log.h>
#include <rikitiki/types.h>

namespace rikitiki {
    namespace websocket {

        WebsocketProcess::WebsocketProcess(WebsocketContext *ctx) : context(ctx), ongoingMessage(0) {

        }

        WebsocketProcess::~WebsocketProcess() {};

        bool WebsocketProcess::OnConnect() { return true; }

        bool WebsocketProcess::OnReady() { return true; }

        void WebsocketProcess::OnClose() {}

        bool WebsocketProcess::OnReceiveText(const char *, size_t) { return true; }

        bool WebsocketProcess::OnReceiveBinary(const unsigned char *, size_t) { return true; }

        bool WebsocketProcess::OnReceiveMessage(const Message &message) {
            if (message.opCode & OpCode::Text)
                return OnReceiveText((const char *) &message.data[0], message.data.size());
            else if (message.opCode & OpCode::Binary)
                return OnReceiveBinary(&message.data[0], message.data.size());

            return true;
        }

        bool WebsocketProcess::OnReceiveFrame(const Frame &frame) {
            if (ongoingMessage == 0) {
                ongoingMessage = new Message(frame);
            } else {
                ongoingMessage->Ingest(frame);
            }

            bool rtn = true;
            if (frame.info.isFinished) {
                rtn = OnReceiveMessage(*ongoingMessage);
                delete ongoingMessage;
                ongoingMessage = 0;
            }

            return rtn;
        }


        WebsocketRoute::WebsocketRoute(const rikitiki::string &r) : route(r) {}

        WebsocketProcess *WebsocketRoute::Handle(WebsocketContext *ctx) {
            bool shouldAttempt = strcmp(route.c_str(), ctx->URI()) == 0;

            if (shouldAttempt) {
                auto rtn = CreateProcessor(ctx);
                if (rtn->OnConnect()) {
                    //LOG(Web, Verbose) << "Using websocket " << route << std::endl;
                    return rtn;
                }
                delete rtn;
            }
            return 0;
        }

    }
}
