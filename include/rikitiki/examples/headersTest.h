/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include <rikitiki/rikitiki>
#include <rikitiki/mongoose/server.h>
#include <rikitiki/configuration/configuration>


using namespace rikitiki;
using namespace rikitiki::mongoose;

namespace rikitiki {
  namespace examples {

    /**
     * This example module exposes how to work with headers and cookies. 
     */
    struct HeadersTestModule : public WebModule {
      void Register(Server& server){
	typedef HeadersTestModule T;
	server.AddHandler( CreateRoute<>::With(this, RT_STRING_LITERAL"/headers", &T::headers) );
        server.AddHandler(CreateRoute<rikitiki::string, rikitiki::string>::With(this, RT_STRING_LITERAL"/cookies/{name}/{value}", &T::set_cookie));
        server.AddHandler(CreateRoute<>::With(this, RT_STRING_LITERAL"/cookies", &T::cookies));
      }

      void headers(ConnContextRef ctx){
	ctx << ContentType::text_plain
	    << "Headers: \n";
	for(auto header : ctx->Request.Headers())
	  ctx << header.first << ": " << header.second << "\n";
      }

      void cookies(ConnContextRef ctx){
           if (ctx->Request.QueryString()[RT_STRING_LITERAL"name"].size())
             ctx << Cookie(ctx->Request.QueryString()[RT_STRING_LITERAL"name"],
			   ctx->Request.QueryString()[RT_STRING_LITERAL"value"]);
    
	ctx << ContentType::text_plain
	    << "Cookies: \n";

	for(auto cook : ctx->Request.Cookies())
	  ctx << cook.first << ": " << cook.second << "\n";
	ctx << "Headers: \n";
	for(auto header : ctx->Request.Headers())
	  ctx << header.first << ": " << header.second << "\n";
      }

      void set_cookie(ConnContextRef ctx, const rikitiki::string& name, const rikitiki::string& value){
		  ctx << Cookie(name, value);
			  cookies(ctx);
      }
    };
  }
}
