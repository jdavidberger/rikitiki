#include <rikitiki/rikitiki.h>
#include <rikitiki/mongoose/server.h>
#include <utils/config.h>

using namespace rikitiki;
using namespace rikitiki::mongoose;

struct HelloWorldModule {
  void Register(Server& server){
    server.AddHandler( CreateRoute<>::With(this, "/hello") );
    server.AddHandler( CreateRoute<int>::With(this, "/{number}") );
    server.AddHandler( CreateRoute<std::string>::With(this, "/{word}") );
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
#ifdef USE_MONGOOSE

int main(){
  MongooseServer server(5000);
  HelloWorldModule module;
  server.Register(module);
  server.Start();
  while(true){
    sleep(1000);
  }
  return 0;
}

#endif

