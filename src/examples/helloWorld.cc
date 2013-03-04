#include <rikitiki.h>
#include <mongoose/server.h>
#include <utils/config.h>

#ifdef USE_CTEMPLATE
#include <ctemplate/connContext_ext.h>
#endif

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
    #ifdef USE_CTEMPLATE
    ctemplate::TemplateDictionary td("example.tpl");
    td.SetValue("bg", bg);
    const std::string& content = ctx.Post()["textarea"];
    td.SetValue("content", content.size() == 0 ? "Hello world!" : content);
    ctx << td;
    #else    
    ctx << "Rikitiki compiled to not use ctemplates; change this option in the root CMakeLists.txt file";
    #endif
  }  
};

#ifdef USE_MONGOOSE

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

#endif

