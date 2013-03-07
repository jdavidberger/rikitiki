/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include <rikitiki/rikitiki.h>
#include <rikitiki/mongoose/server.h>
#include <rikitiki/ctemplate/connContext_ext.h>
#include <rikitiki/ctemplate/headerFooterPreprocessor.h>
#include <utils/config.h>
using namespace rikitiki;
using namespace rikitiki::mongoose;
using namespace rikitiki::ctemplates;

namespace rikitiki {
  namespace examples {
struct HeadersTestModule {
  void Register(Server& server){
    server.AddPreprocessor( new HeaderFooterPreprocessor() );
    typedef HeadersTestModule T;
    server.AddHandler( CreateRoute<>::With(this, "/headers", &T::headers) );
    server.AddHandler( CreateRoute<std::string, std::string>::With(this, "/cookies/{name}/{value}", &T::set_cookie) );
    server.AddHandler( CreateRoute<>::With(this, "/cookies", &T::cookies) );
  }

  void headers(ConnContext& ctx){
    ctx << ContentType::text_plain
        << "Headers: \n";
    foreach(it, ctx.Headers())
      ctx << it->first << ": " << it->second << "\n";
  }

  void cookies(ConnContext& ctx){
    if(ctx.QueryString()["name"].size())
      ctx << Cookie(ctx.QueryString()["name"], ctx.QueryString()["value"]);
    
    ctx << ContentType::text_plain
        << "Cookies: \n";

    foreach(it, ctx.Cookies())
      ctx << it->first << ": " << it->second << "\n";
    ctx << "Headers: \n";
    foreach(it, ctx.Headers())
      ctx << it->first << ": " << it->second << "\n";
  }

  void set_cookie(ConnContext& ctx, const std::string& name, const std::string& value){
    ctx << Cookie(name, value)
	<< "<a href='/cookies/test/test'>test</a>\n";
  }
};
  }
}

using namespace rikitiki::examples;
#ifdef USE_MONGOOSE

int main(){
  MongooseServer server(5000);
  HeadersTestModule module;

  server.Register(module);
  server.Start();
  while(true){
    sleep(1000);
  }
  return 0;
}

#endif

