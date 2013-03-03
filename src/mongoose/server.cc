/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include "server.h"

namespace rikitiki {
  namespace mongoose {

MongooseServer::MongooseServer(int _port) : Server(_port)  { 
  ctx = 0;
}

static void* _handler(enum mg_event event, struct mg_connection *conn) {
  const struct mg_request_info *request_info = mg_get_request_info(conn);
  return ((Server*)(request_info->user_data))->Handle(event, conn);
}

void MongooseServer::Start() {
  const char *options[] = {"listening_ports", "5000", 
			   NULL};
  ctemplate::Template::SetTemplateRootDirectory(CTEMPLATE_ROOT_DIRECTORY);
  ctx = mg_start(_handler, this, options);
}

void MongooseServer::Stop() {
  mg_stop(ctx);
  ctx = 0;
}
  }
}
