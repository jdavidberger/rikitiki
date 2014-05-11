/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include <rikitiki/rikitiki>

using namespace rikitiki;

namespace rikitiki {
  namespace examples {
    /** Simple example module of routing
     */
    struct HelloWorldModule {
      void Register(Server& server){
		server.AddHandler( CreateRoute<>::With(this, L"/hw/hello") );
                server.AddHandler(CreateRoute<int>::With(this, L"/hw/{number}"));
                server.AddHandler(CreateRoute<std::wstring>::With(this, L"/hw/{word}"));
      }

      void operator()(ConnContextRef ctx){
		ctx << "Hello world!";
      }

      void operator()(ConnContextRef ctx, int number){
		ctx	<< "Number: " << number;
      }

      void operator()(ConnContextRef ctx, const std::wstring& word){
		ctx << "Word: " << word;
      }  
    };
  }
}
