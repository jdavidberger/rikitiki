/* Copyright (C) 2012-2013 Justin Berger
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include <rikitiki/mongoose/mongoose>
#include <rikitiki/configuration/configuration>
#include <rikitiki\websocket\websocketProcess.h>
#include <rikitiki\server.h>
#include <sstream>
#include <signal.h>
#include <mongoose.h>
#include <condition_variable>
#include <thread>

namespace rikitiki {
     namespace mongoose {

          MongooseServer::MongooseServer(int _port) : port(_port)  {
               ctx = 0;
#ifdef RT_USE_CONFIGURATION
               if(Configuration::Global().exists("mongoose")){
                    Configuration::Global().getRoot()["mongoose"].lookupValue("document_root", DocumentRoot);
               }
#endif
          }

          static MongooseServer* getServer(const struct mg_connection *conn) {
               const struct mg_request_info *request_info = mg_get_request_info(const_cast<struct mg_connection*>(conn));
               return((MongooseServer*)(request_info->user_data));
          }

          static int _handler(struct mg_connection *conn) {
               MongooseServer* server = getServer(conn);
               std::shared_ptr<mongoose::MongooseConnContext> ctx(new mongoose::MongooseConnContext(server, conn));
               return server->Handle(ctx) ? 1 : 0;
          }

          static int _wsHandler(const struct mg_connection *conn) {
               MongooseServer* server = getServer(conn);
               auto ctx = new mongoose::MongooseWebsocketContext(server, conn);
               websocket::WebsocketProcess* process = server->HandleWs(ctx);
               if (process) {
                    server->processes[(void*)conn] = process;
                    return 0;
               }
               return 1;
          }

          static void _wsReady(struct mg_connection *conn) {
               MongooseServer* server = getServer(conn);
               auto process = server->processes[conn];
               if (process) {
                    auto ctx = dynamic_cast<mongoose::MongooseWebsocketContext*>(process->context);
                    assert(ctx);
                    ctx->setConnection(conn);
                    server->processes[conn]->OnReady();
               }
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

          static int _wsReceive(struct mg_connection *conn, int bits, char* data, size_t length) {
               MongooseServer* server = getServer(conn);
               auto process = server->processes[conn];
               assert(process);
               websocket::Frame frame(bits, (unsigned char*)data, length);
			   
			   if (frame.info.OpCode() & websocket::OpCode::Close) {				   
				   process->OnClose();
				   delete process;
				   server->processes.erase(conn);
				   return true;
			   }
               return process->OnReceiveFrame(frame) ? 1 : 0;
          }

          static volatile bool quit;
          static std::condition_variable quit_cond;

          static void forcequit() {
               quit = 1;
               quit_cond.notify_all();
          }
          static void signal_handler(int sig){
               if (quit == 1)
                    signal(SIGINT, SIG_DFL); // Don't trap the signal forever; the next SIGINT should abort. 
               forcequit();
          }

          void MongooseServer::Run(){
               quit = 0;
               signal(SIGINT, signal_handler);
               Start();

               // Possible race condition -- If we catch sigint and then a new server
               // starts up within the sleep window, this server will not stop. 
               // Won't address until a use case for that comes up though. 

               std::mutex m;
               std::unique_lock<std::mutex> l(m);
               while (!quit){ quit_cond.wait(l); }

               LOG(Mongoose, Debug) << "Caught sigint, shutting down server " << (void*)this << std::endl;
               Stop();
          }


          void MongooseServer::Start() {
               mg_callbacks callbacks;
               memset(&callbacks, 0, sizeof(callbacks));
               callbacks.begin_request = &_handler;
               callbacks.websocket_connect = &_wsHandler;
               callbacks.websocket_ready = &_wsReady;
               callbacks.websocket_data = &_wsReceive;
			   
               std::stringstream _port;
               _port << port;
               std::string __port = _port.str();
               options.clear();
               options.push_back("listening_ports"), options.push_back(__port.c_str());
               if (DocumentRoot.size())
                    options.push_back("document_root"), options.push_back(&DocumentRoot[0]);
               options.push_back(NULL);

               ctx = mg_start(&callbacks, this, &options[0]);
          }

          void MongooseServer::Stop() {
               mg_stop(ctx);
               forcequit();
               ctx = 0;
          }
     }
}
