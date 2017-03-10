/* Copyright (C) 2012-2013 Justin Berger
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include <rikitiki/mongoose/mongoose>
#include <mongoose.h>

#include <rikitiki/configuration/configuration>
#include <rikitiki/websocket/websocketProcess.h>
#include <rikitiki/server.h>

#include <sstream>

#pragma warning(disable:4062)

#include <signal.h>

#pragma warning(default:4062)

#include <rikitiki/http/helpers/SimpleRequestClient.h>
#include <string.h>

#pragma warning(disable:4265)

#include <condition_variable>
#include <thread>
#include <rikitiki/mongoose/server.h>
#include <rikitiki/mongoose/connContext.h>

#pragma warning(default:4265)

namespace rikitiki {
    namespace mongoose {

        std::future<std::shared_ptr<Response>> MongooseServer::ProcessRequest(Request &request) {
            auto client = new SimpleRequestClient(L"localhost", this->Port());
            client->MakeRequest(request);
            return client->future();
        }

        uint16_t MongooseServer::Port() { return port; }

        MongooseServer::MongooseServer(uint16_t _port) : port(_port) {
            ctx = 0;
#ifdef RT_USE_CONFIGURATION
            if(Configuration::Global().exists("mongoose")){
                 Configuration::Global().getRoot()["mongoose"].lookupValue("document_root", DocumentRoot);
            }
#endif
        }

        static MongooseServer *getServer(const struct mg_connection *conn) {
            return ((MongooseServer *) (conn->mgr->user_data));
        }

        static void _handler(struct mg_connection *conn, int ev, void *ev_data) {
            MongooseServer *server = getServer(conn);
            LOG(Mongoose, Debug) << "Incoming request " << ev << " on " << server << std::endl;

            switch (ev) {
                case MG_EV_HTTP_REQUEST: {
                    struct http_message *hm = (struct http_message *) ev_data;
                    ConnContextRef_<ConnContext> ctx(new mongoose::MongooseConnContext(server, conn, hm));
                    if (!server->Handle(ctx)) {

                    }
                }
                    break;
                default:
                    break;
            }
        }

#ifdef RT_USE_WEBSOCKET
        using namespace websocket;
        WebsocketProcess* MongooseServer::HandleWs(ConnectionHandle conn) {
             auto ctx = new mongoose::MongooseWebsocketContext(this, (const struct mg_connection *)conn);
             websocket::WebsocketProcess* process = HandleWs(ctx);
             if (process) {
                  return processes[conn] = process;
             }
             return 0;
        }

        int MongooseServer::_wsHandler(const struct mg_connection *conn) {
             MongooseServer* server = getServer(conn);
             if (server->HandleWs((ConnectionHandle)conn) == 0)
                  return 1;
             return 0;
        }

        void MongooseServer::_wsReady(struct mg_connection *conn) {
             MongooseServer* server = getServer(conn);
             server->OnReady((ConnectionHandle)conn);
        }

        int MongooseServer::_wsReceive(struct mg_connection *conn, int bits, char* data, size_t length) {
             MongooseServer* server = getServer(conn);
             auto process = server->processes[conn];
             assert(process);
             websocket::Frame frame((unsigned char)bits, (unsigned char*)data, length);

             if (frame.info.OpCode() & websocket::OpCode::Close) {
                  process->OnClose();
                  delete process;
                  server->processes.erase(conn);
                  return true;
             }
             return process->OnReceiveFrame(frame) ? 1 : 0;
        }

        void MongooseServer::Close(websocket::WebsocketContext* _ctx) {
             auto ctx = dynamic_cast<MongooseWebsocketContext*>(_ctx);
             mg_connection * conn = ctx->conn;
             auto process = processes[conn];
             assert(process);
             process->OnClose();
             delete process;
             processes.erase(conn);
        }
#endif


        static volatile bool quit;
        static std::condition_variable quit_cond;

        static void forcequit() {
            quit = 1;
            quit_cond.notify_all();
        }

        static void signal_handler(int sig) {
            UNREFERENCED_PARAMETER(sig);
            if (quit == 1)
                signal(SIGINT, SIG_DFL); // Don't trap the signal forever; the next SIGINT should abort.
            forcequit();
        }

        void MongooseServer::WaitForStop() {
            quit = 0;
            signal(SIGINT, signal_handler);

            // Possible race condition -- If we catch sigint and then a new server
            // starts up within the sleep window, this server will not stop.
            // Won't address until a use case for that comes up though.

            std::mutex m;
            std::unique_lock<std::mutex> l(m);
            while (!quit) { quit_cond.wait(l); }

            LOG(Mongoose, Debug) << "Caught sigint, shutting down server " << (void *) this << std::endl;
            Stop();
        }

        void MongooseServer::Run() {
            Start();
            WaitForStop();
        }


        void MongooseServer::Start() {
            runThread = std::thread([&] {
                mg_mgr mgr;
                mg_mgr_init(&mgr, NULL);

#ifdef USE_WEBSOCKET
                callbacks.websocket_connect = &_wsHandler;
                callbacks.websocket_ready = &_wsReady;
                callbacks.websocket_data = &_wsReceive;
#endif
                std::stringstream _port;
                _port << port;
                std::string __port = _port.str();


                LOG(Mongoose, Debug) << "Starting Mongoose web server " << (void *) this << std::endl;

                /*
                struct mg_serve_http_opts s_http_server_opts;
                if (DocumentRoot.size())
                    s_http_server_opts.document_root = DocumentRoot.c_str();
*/

                auto nc = mg_bind(&mgr, __port.c_str(), _handler);
                mg_set_protocol_http_websocket(nc);
                mgr.user_data = this;
                while (!quit) {
                    mg_mgr_poll(&mgr, 1000);
                }
                mg_mgr_free(&mgr);
            });
            /* Cleanup */


            //        ctx = mg_start(&callbacks, this, &options[0]);
        }

        void MongooseServer::Stop() {
            forcequit();
            if (runThread.joinable())
                runThread.join();
            ctx = 0;
        }
    }
}
