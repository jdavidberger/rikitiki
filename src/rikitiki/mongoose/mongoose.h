/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */
#pragma once
#include "server.h"
#include "connContext.h"

/**\page mongoose Mongoose Integration
   Integration into mongoose happens in a slightly different manner than with apache. Rather than loading into an existing server, we compile an executable which acts as standalone http server. MongooseServer is a thin wrapper around native mongoose functionality which exposes both methods to register modules against it, as well as starting and stopping the server. 

Below is a simple example that registers a module and runs mongoose port 5000:
~~~~~~~
int main(){
  int port = 5000;
  MongooseServer server(port);
  <module-struct> module;
  server.Register(module);
  server.Run();
  return 0;
}
~~~~~~~

\sa rikitiki::mongoose::MongooseServer, helloWorld.h
 */
