/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */
#include "helloWorld.h"
#include "advanced.h"
#include "rest.h"
#include "headersTest.h"

using namespace rikitiki;
using namespace rikitiki::mongoose;
using namespace rikitiki::ctemplates;
using namespace rikitiki::examples;

int main(){
  MongooseServer server(5000);

  HelloWorldModule hw;
  AdvancedModule adv;
  RestModule rest;
  HeadersTestModule headers;

  server.Register(hw);
  server.Register(adv);
  server.Register(rest);
  server.Register(headers);
  server.Run();
  
  return 0;
}

/** \page examples Examples 
- rikitiki::examples::HelloWorldModule - A simple, one page example which shows the basic idea behind routing.
- rikitiki::examples::AdvancedModule - A more advanced example which shows how to use
    - CTemplates
    - CTemplate preprocessors
    - GET/POST routes
    - Retrieving post and querystring data. 
- rikitiki::examples::HeadersTestModule - Displays the headers and cookies of the page.
- rikitiki::examples::RestModule - Example of RESTful service
*/


