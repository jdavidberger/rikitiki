/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */
#include <rikitiki/mongoose/server.h>
#include <mxcomp/log.h>
#include "${imodule}.h"

using namespace rikitiki::mongoose;
using namespace rikitiki::examples;
int main(){
  mxcomp::log::SetLogLevel(mxcomp::log::Debug);
  mxcomp::log::SetLogStream(std::cerr);

  MongooseServer server(5000);
  ${modstruct} module;
  server.Start();
  server.Register(module);
  server.WaitForStop();
  return 0;
}


