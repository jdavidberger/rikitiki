/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */
#pragma once
#include <rikitiki/rikitiki>

using namespace rikitiki;

namespace rikitiki {
  namespace examples {
    /** Simple example module of routing
     */
    struct HelloWorldModule : public rikitiki::WebModule {
      void Register(Server& server) override {
		server.AddHandler( CreateRoute<>::With(this, RT_STRING_LITERAL"/hw/hello") );
                server.AddHandler(CreateRoute<int>::With(this, RT_STRING_LITERAL"/hw/{number}"));
                server.AddHandler(CreateRoute<rikitiki::string>::With(this, RT_STRING_LITERAL"/hw/{word}"));
                server.AddHandler(new StaticContentHandler("/static/", "c:\\"));
      }

      void operator()(ConnContextRef ctx){           
	ctx << "Hello world!!";
      }

      void operator()(ConnContextRef ctx, int number){
		ctx	<< "Number: " << number;
      }

      void operator()(ConnContextRef ctx, const rikitiki::string& word){
		ctx << "Word: " << word;
      }  
    };
  }
}
