/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include <rikitiki/mongoose/mongoose>
#include <rikitiki/configuration/configuration>
#include <sstream>
#include <signal.h>

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

    static void* _handler(enum mg_event event, struct mg_connection *conn) {
      const struct mg_request_info *request_info = mg_get_request_info(conn);

      MongooseServer* server = ((MongooseServer*)(request_info->user_data));

      if(event != MG_NEW_REQUEST)
	return (void*)0;
      
      mongoose::MongooseConnContext ctx(server, event, conn);
      return server->Handle(ctx) ? (void*)1 : (void*)0;
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
      std::stringstream _port;
      _port << port;
      std::string __port = _port.str();
      std::vector<const char*> options;
      options.push_back("listening_ports"), options.push_back(__port.c_str());
      if(DocumentRoot.size())
	options.push_back("document_root"), options.push_back(&DocumentRoot[0]);
      options.push_back(NULL);

      ctx = mg_start(_handler, this, &options[0]);
    }

    void MongooseServer::Stop() {
      mg_stop(ctx);
      ctx = 0;
    }
  }
}
