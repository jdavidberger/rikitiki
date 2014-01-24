/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include <rikitiki/mongoose/mongoose>
#include <rikitiki/configuration/configuration>
#include <sstream>
#include <signal.h>
#include <mongoose.h>

#ifdef _MSC_VER
#include <Windows.h>

void sleep(int waitTime) {
	__int64 time1 = 0, time2 = 0, freq = 0;

	QueryPerformanceCounter((LARGE_INTEGER *)&time1);
	QueryPerformanceFrequency((LARGE_INTEGER *)&freq);

	do {
		QueryPerformanceCounter((LARGE_INTEGER *)&time2);
	} while ((time2 - time1) < waitTime);
}
#else
#include <unistd.h>
#endif

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

    static int _handler(struct mg_connection *conn) {
      const struct mg_request_info *request_info = mg_get_request_info(conn);

      MongooseServer* server = ((MongooseServer*)(request_info->user_data));

      mongoose::MongooseConnContext ctx(server, conn);
      return server->Handle(ctx) ? 1 : 0;
    }

    static volatile sig_atomic_t quit;
    static void signal_handler(int sig){      
      quit=1;
      signal(SIGINT, SIG_DFL); // Don't trap the signal forever; the next SIGINT should abort. 
    }

    void MongooseServer::Run(){
      quit = 0;
      signal(SIGINT, signal_handler);
      Start();

      // Possible race condition -- If we catch sigint and then a new server
      // starts up within the sleep window, this server will not stop. 
      // Won't address until a use case for that comes up though. 
      while(!quit){ sleep(1000); }
      LOG(Mongoose, Debug) << "Caught sigint, shutting down server " << (void*)this << std::endl;
      Stop();
    }


    void MongooseServer::Start() {  
      mg_callbacks callbacks;      
      memset(&callbacks, 0, sizeof(callbacks));
      callbacks.begin_request = &_handler;
      
      std::stringstream _port;
      _port << port;
      std::string __port = _port.str();
      options.clear();
      options.push_back("listening_ports"), options.push_back(__port.c_str());
      if(DocumentRoot.size())
	options.push_back("document_root"), options.push_back(&DocumentRoot[0]);
      options.push_back(NULL);

      ctx = mg_start(&callbacks, this, &options[0]);
    }

    void MongooseServer::Stop() {
      mg_stop(ctx);
      ctx = 0;
    }
  }
}
