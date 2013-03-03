#include "server.h"

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
