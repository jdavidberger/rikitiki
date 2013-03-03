#include <rikitiki.h>
#include <mongoose/server.h>
#include <apache/register.h>
#include <config.h>
using namespace rikitiki;
using namespace rikitiki::mongoose;

struct HelloWorldModule {
  void Register(std::vector<Handler*>& h){
    h.push_back( CreateRoute<>::With(this, "/hello") );
    h.push_back( CreateRoute<int>::With(this, "/{number}") );
    h.push_back( CreateRoute<std::string>::With(this, "/ctemplate/{word}", &HelloWorldModule::ctemplate) );
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

  void ctemplate(ConnContext& ctx, const std::string& bg){
    ctemplate::TemplateDictionary td("example.tpl");
    td.SetValue("bg", bg);
    td.SetValue("content", "Hello world!");
    ctx << td;
  }  
};

int main(){
  MongooseServer server(5000);
  HelloWorldModule module;
  LOG(Main, Verbose) << "Testing " << std::endl;
  server.Register(module);
  server.Start();
  while(true){
    sleep(1000);
  }
  return 0;
}

