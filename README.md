# rikitiki
Build C++ web server modules that allow easy routing and deployment.

rikitiki is a project aimed at adding developer convenience to writing web server modules.

It provides integration into mongoose and apache, and contains easy to configure and use routing functionality. Optionally, it includes support for the ctemplates library as a templating engine.

## Features

Current Features:

*   Fully supports apache and mongoose in a consistent C++ interface
*   Easy routing that parses the URL and passes parsed members to your own handler function
*   Out of the box support for CTemplates and Json
*   Support for reading and writing custom types to available formats (Json for now)

Upcoming Features for v0.2:

*   Out of the box support for XML and SOAP
*   Increased cookie support to handle standard cookie attributes (in latest)
*   Easy integration of JSON content types (in latest)
*   REST registration (in latest)

## Example

```C++
#include <rikitiki/rikitiki.h>
#include <rikitiki/mongoose/server.h>

struct HelloWorldModule {
  void Register(Server& server){
    server.AddHandler( CreateRoute<>::With(this, "/hw/hello") );
    server.AddHandler( CreateRoute<int>::With(this, "/hw/{number}") );
    server.AddHandler( CreateRoute<std::string>::With(this, "/hw/{word}") );
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
```

[See all examples](doxygen/examples.html)
