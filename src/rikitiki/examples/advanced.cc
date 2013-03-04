#include <rikitiki/rikitiki.h>
#include <rikitiki/mongoose/server.h>
#include <utils/config.h>
#include <rikitiki/ctemplate/connContext_ext.h>
#include <rikitiki/ctemplate/headerFooterPreprocessor.h>
using namespace rikitiki;
using namespace rikitiki::mongoose;
using namespace rikitiki::ctemplates;

struct CTemplateExampleModule {

  void Register(Server& server){
    server.AddPreprocessor( new HeaderFooterPreprocessor() );
    typedef CTemplateExampleModule T;
    server.AddHandler( CreateRoute<>::With(this, "/", &T::Index) );
    server.AddHandler( CreateRoute<>::With(this, "/post", &T::Get,  ConnContext::GET ) );
    server.AddHandler( CreateRoute<>::With(this, "/post", &T::Post, ConnContext::POST ));
    server.AddHandler( CreateRoute<>::With(this, "/qs", &T::QueryString));
  }

  void Index(ConnContext& ctx){
    ctemplate::TemplateDictionary td("commands.tpl");
    for(unsigned int i = 0;i < ctx.server->handlers.size();i++){
	if(ctx.server->handlers[i]->visible()){
	  ctemplate::TemplateDictionary* row = td.AddSectionDictionary("HANDLER");
	  row->SetValue("name", ctx.server->handlers[i]->name());
	  row->SetValue("description", ctx.server->handlers[i]->desc());
	}
    }
    ctx << td;  
  }
  
  void makePage(ConnContext& ctx, const std::string& message){
    ctemplate::TemplateDictionary td("example.tpl");
    td.SetValue("message", message.size() ? message : "You didn't enter anything!");    
    ctx << td;      
  }

  void Post(ConnContext& ctx){
    makePage(ctx, ctx.Post()["message"]);
  }  

  void Get(ConnContext& ctx){
    makePage(ctx, "Please enter a message below");
  }  
  
  void QueryString(ConnContext& ctx){
    ctx << ContentType::application_javascript 
	<< "{";
    foreach(it, ctx.QueryString()){
      ctx << "\t '" << it->first << "': '" << it->second << "', \n";
    }
    ctx << "}";
  }
};

#ifdef USE_MONGOOSE

int main(){
  MongooseServer server(5000);
  CTemplateExampleModule module;
  LOG(Main, Info) << "Remember to run this from the html directory if you want styles to work!" << std::endl;
  server.Register(module);
  server.Start();
  while(true){
    sleep(1000);
  }
  return 0;
}

#endif

