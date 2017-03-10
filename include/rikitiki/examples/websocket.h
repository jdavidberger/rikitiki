/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include <rikitiki/rikitiki>
#include <rikitiki/mongoose/mongoose>
#include <rikitiki/websocket/websocket>
#include <iostream>
#include <fstream>

using namespace rikitiki;
using namespace rikitiki::mongoose;
using namespace rikitiki::websocket;

namespace rikitiki {
  namespace examples {

    struct Image : public WebsocketProcess {
      std::vector<char> buffer;
      Image(WebsocketContext* ctx) : WebsocketProcess(ctx) {}
      virtual bool OnReady() {
	std::ifstream input("c:\\test.png", std::ios::binary);

        if (input.is_open()){
             input.seekg(0, input.end);
             auto length = input.tellg();
             input.seekg(0, input.beg);
             buffer.resize((std::size_t)length);
             input.read(&buffer[0], length);
             input.close();
        }

	return true;
      }  

      virtual bool OnReceiveText(const char*, size_t ) {
	context->Write(&buffer[0], buffer.size());
	return true;
      }

      virtual bool OnPing() {
	context->Write(&buffer[0], buffer.size());
	return true;
      }
    };

    struct Chat : public WebsocketProcess {
    Chat(WebsocketContext* ctx) : WebsocketProcess(ctx) {}
      virtual bool OnReady() {
	context->Write(std::string("Hi! This is a fragmented frame (or it should be)"));
	return true;
      }
      virtual bool OnReceiveText(const char* buffer, size_t len) {
	context->Write(std::string(buffer, buffer + len));
	return true;
      }
    };

    /** Simple example module of routing
     */
    struct WebsocketModule : public rikitiki::websocket::WebModule {
      void Register(websocket::Server& server){
           server.AddWsHandler(CreateWsRoute<Chat>::With(RT_STRING_LITERAL"/chat"));
           server.AddWsHandler(CreateWsRoute<Image>::With(RT_STRING_LITERAL"/image"));
      }
    };
  }
}
