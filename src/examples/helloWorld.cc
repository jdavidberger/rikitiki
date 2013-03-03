#include <rikitiki.h>
#include <mongoose/server.h>
#include <apache/register.h>
using namespace rikitiki;
using namespace rikitiki::mongoose;

struct HelloWorldModule {
  void Register(std::vector<Handler*>& h){
    h.push_back( CreateRoute<>::With(this, "/hello") );
    h.push_back( CreateRoute<int>::With(this, "/{number}") );
    h.push_back( CreateRoute<std::string>::With(this, "/{word}") );
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

