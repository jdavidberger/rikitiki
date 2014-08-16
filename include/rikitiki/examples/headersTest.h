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
	server.AddHandler( CreateRoute<>::With(this, L"/headers", &T::headers) );
        server.AddHandler(CreateRoute<std::wstring, std::wstring>::With(this, L"/cookies/{name}/{value}", &T::set_cookie));
        server.AddHandler(CreateRoute<>::With(this, L"/cookies", &T::cookies));
      }

      void headers(ConnContextRef ctx){
	ctx << ContentType::text_plain
	    << "Headers: \n";
	for(auto header : ctx->Request.Headers())
	  ctx << header.first << ": " << header.second << "\n";
      }

      void cookies(ConnContextRef ctx){
           if (ctx->Request.QueryString()[L"name"].size())
             ctx << Cookie(ctx->Request.QueryString()[L"name"], 
			   ctx->Request.QueryString()[L"value"]);
    
	ctx << ContentType::text_plain
	    << "Cookies: \n";

	for(auto cook : ctx->Request.Cookies())
	  ctx << cook.first << ": " << cook.second << "\n";
	ctx << "Headers: \n";
	for(auto header : ctx->Request.Headers())
	  ctx << header.first << ": " << header.second << "\n";
      }

      void set_cookie(ConnContextRef ctx, const std::wstring& name, const std::wstring& value){
		  ctx << Cookie(name, value);
			  cookies(ctx);
      }
    };
  }
}
