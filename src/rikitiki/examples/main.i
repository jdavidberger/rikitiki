/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */
#include <rikitiki/mongoose/server.h>
#include "${imodule}.cc"

using namespace rikitiki::mongoose;
using namespace rikitiki::examples;
int main(){
  MongooseServer server(5000);
  ${modstruct} module;
  server.Register(module);
  server.Start();
  while(true){
    sleep(1000);
  }
  return 0;
}


