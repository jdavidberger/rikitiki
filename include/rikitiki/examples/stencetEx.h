/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */
#pragma once

#include <rikitiki/rikitiki>
#include <rikitiki/mongoose/mongoose>
#include <stencet/stencet>
#include <mxcomp/variant.h>

using namespace rikitiki;
using namespace rikitiki::mongoose;
using namespace stencet;

namespace rikitiki {
  namespace examples {

    struct StencetExModule {
      void Register(Server& server){
	Template::Templates()["hw"].Parse("<html><body>{{message}}</body></html>");
	server.AddHandler( CreateRoute<>::With(this, "/stencet") );
      }

      void operator()(ConnContext& ctx){	
	Variant vm;
	vm["message"] = "Hello world!";
	ctx << Template::ByName("hw").With(vm);
      }
    };
  }
}

