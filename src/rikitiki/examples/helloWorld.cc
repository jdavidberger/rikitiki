/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include <rikitiki/rikitiki.h>
#include <rikitiki/mongoose/server.h>
#include <utils/config.h>

using namespace rikitiki;
using namespace rikitiki::mongoose;

namespace rikitiki {
  namespace examples {
    /** Simple example module of routing
     */
    struct HelloWorldModule {
      void Register(Server& server){
	server.AddHandler( CreateRoute<>::With(this, "/hw/hello") );
	server.AddHandler( CreateRoute<int>::With(this, "/hw/{number}") );
	server.AddHandler( CreateRoute<std::string>::With(this, "/hw/{word}") );
      }

      void operator()(ConnContext& ctx){
	ctx << "Hello world!";
      }

      void operator()(ConnContext& ctx, int number){
	ctx << "Number: " << number;
      }

      void operator()(ConnContext& ctx, const std::string& word){
	ctx << "Word: " << word;
      }  
    };
  }
}

