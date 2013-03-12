/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */
#include <rikitiki/examples/helloWorld.h>
#ifdef RT_USE_CTEMPLATE
#include <rikitiki/examples/advanced.h>
#endif
#ifdef RT_USE_REST
#include <rikitiki/examples/rest.h>
#include <rikitiki/examples/restAdv.h>
#endif
#include <rikitiki/examples/headersTest.h>
#include <rikitiki/examples/multiType.h>

using namespace rikitiki;
using namespace rikitiki::mongoose;
using namespace rikitiki::examples;

extern bool log_config_loaded;

int main(){
  MongooseServer server(5000);

  HelloWorldModule hw;
  server.Register(hw);
#ifdef RT_USE_CTEMPLATE
  AdvancedModule adv;
  server.Register(adv);
#endif
#ifdef RT_USE_REST
  RestModule rest;
  server.Register(rest);
  RestAdvModule restAdv;
  server.Register(restAdv);
#endif
  MultiTypeModule mtm;
  server.Register(mtm);
  HeadersTestModule headers;
  server.Register(headers);

  server.Run();
  
  return 0;
}

