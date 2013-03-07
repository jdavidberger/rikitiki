/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include <rikitiki/server.h>
#include <rikitiki/connContext.h>
#include <rikitiki/routing.h>

/**\mainpage rikitiki Documentation

\tableofcontents

\section Dependencies Dependencies

These dependencies are available through most package managers. 

- libconfig 1.49
- curl 
- apache 2.4 (Can disable by setting USE_APACHE to 0 in the CMakeLists.txt file at the root)
- Compiler with c++11 support
    - Variadic templates
    - decltype
    - Tested on gcc 4.7.2, probably works on previous versions

\section installation Installation 

\subsection download Download the source 
~~~~~~~
hg clone http://hg.code.sf.net/p/rikitiki/code rikitiki-code
~~~~~~~

\subsection build Build 
~~~~~~~
cd rikitiki-code
mkdir bin
cd bin
cmake ..
make
~~~~~~~


\subsection run Run the example 
~~~~~~~
./helloWorld
~~~~~~~


\subsection copypaste Or copy-paste this 
~~~~~~~
hg clone http://hg.code.sf.net/p/rikitiki/code rikitiki-code
cd rikitiki-code
mkdir bin
cd bin
cmake ..
make
./helloWorld
~~~~~~~

\section QuickStart QuickStart 

\subsection module Creating a module 

The basic idea behind rikitiki is that each module that can push some information to a webpage should just be a collection of handlers. Modules are simply structs with a Register function:

~~~~~~
using namespace rikitiki;
struct HelloWorldModule {
   void Register(std::vector<Handler*>& h) { }
};
~~~~~~

will define a functional, albeit useless, module. 

To make it functional, we need to add handlers. Most of the time, you want to add routes, which you can do like so:

~~~~~~
using namespace rikitiki;
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
~~~~~~

This will create three entry points for your site. When called, they will simply display the text streamed to the ConnContext object. 

\subsection runit Running a web server 

A module on it's own, even with entry points, is still a little useless -- to make it run we have to incorporate it into either an apache module or create an executable for it on top of mongoose. Neither is particularly hard to do.

\subsubsection mongoose Mongoose 

We can instantiate and run a mongoose server against a module with:

~~~~~~
using namespace rikitiki::mongoose;
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
~~~~~~

Running this executable will start a server on port 5000. Test each entry point with:

- http://localhost:5000/hello
- http://localhost:5000/42
- http://localhost:5000/testing

\subsection apache Apache 

Running the module in apache is more involved, but not by much. A macro does most of the code set-up for us:

~~~~~~
RegisterApacheHandler(helloWorld, HelloWorldModule);
~~~~~~

when compiled to a shared object, this will expose a 'helloWorld_module' to apache. Supposing we compiled a 'mod_helloWorld.so' shared object, we need to take that file and copy or link it to apaches modules directory. On most linux systems, that is going to be /etc/httpd/modules. 

Then we need to add the following line to the httpd.conf file (usually at /etc/httpd/conf/httpd.conf on linux systems):

~~~~~~
LoadModule helloWorld_module modules/mod_helloWorld.so
~~~~~~

or 

~~~~~~
LoadModule helloWorld_module modules/mod_helloWorld.dll
~~~~~~

for windows. 

Run or restart apache, and the module should be loaded and available:

- http://localhost/hello
- http://localhost/42
- http://localhost/testing

Clearly though, these addresses are dependent on the particular apache configuration.     

\section config Configuration 

rikitiki looks for and loads a configuration file. It uses this configuration file to determine logging levels as well as setting the directory to look for ctemplate templates in. 

\subsection loadorder Load order 

Configuration files are looked for in the following order:

- <assembly-path>/<assembly-name>.cfg
- <assembly-path>/../conf/<assembly-name>.cfg
- /etc/rikitiki/conf/<assembly-name>.cfg
- <assembly-path>/default.cfg
- <assembly-path>/../conf/default.cfg
- /etc/rikitiki/conf/default.cfg

It is recommended that you place yours in the '/etc/rikitiki/conf' directory so that it doesn't matter where the module is run from.

\subsection config Config format 
The config files look like this:

~~~~~~~
ctemplate_root = "$(CONFIG_PATH)/../html/ctemplates";

log = {
    levels = ( 
       { category = "Logging"; level = "Error" },
          { category = "Server"; level = "Verbose" } 
    );
};
~~~~~~~

See [the libconfig site](http://www.hyperrealm.com/libconfig/) for further documentation.

\subsection variables Available variables 
Values between '$(' and ')' are treated as variables and expanded out. 

The configuration engine recognizes a few built-in variables:

- CONFIG_PATH - The path the config file is found at. 
- BIN_NAME    - The name of the executing library or executable. 
- BIN_PATH    - The path to the executing library or executable. 

\section modules Modules 

Modules can be anything with an appropriate registration function to register their handlers to the server. This registration function will only be called once per server object, and it's state is persisted for the lifetime of the server -- so be aware that different handlers might fire at the same time depending on web traffic.

The registration function has this signature: 

~~~~~~~
void Register(rikitiki::Server& server);
~~~~~~~

In this function, you should set up whatever state is required, and add the appropriate handlers to the server

\subsection handlers Handlers 

Handlers are a common theme for these kinds of applications. The meat of the handler functionality is the 

~~~~~~~
virtual bool Handle(ConnContext& ctx);
~~~~~~~

function, and predictably, it is this function that processes each request. The return value indicates whether the handler actually handled the request (true) or not (false). 

Handlers are tried in the order they are added in. Once one handler handles the request, the rest are not executed. 

\subsection routes Routes 

Routes are implementations of handlers that check the incoming request against a route string, and decide whether or not they should handle the request. They then delegate out the actual processing of the request to a user specified function. 

One of the convenient features of routes in rikitiki is that they provide functionality for basic parsing of the url, much like scanf, and pass the parsed values to the user provided function. 

\subsubsection createroute CreateRoute::With 

As a matter of convenience, we want to let C++ figure out as many types as it can. With this in mind, there is a set of helper functions that create routes for us. These calls tend to look like this:

~~~~~~~
CreateRoute<args...>::With(this, "/this/is/your/route", &MyClass::MyFunction)
~~~~~~~

Note that you can pass an instance of anything in, it doesn't have to be a module or have any signature other than the one you specified for the route to call. 

This function take an optional fourth argument, with which you can specify the http request method type. The default is to accept any request method, but it can often times be useful to specify POST or GET -- for instance, you could set up two handlers with the same route, but where one function is meant specifically to handle form POST requests. 

An overload exists for which you don't have to specify the function. If you use this overload, it defaults to using 'operator()' as the function to route to. 

Do note that the object you pass as the first argument will not be copied, and that reference is expected to be around as long as the server is active. 

\subsubsection routestring Route String 

The second argument that CreateRoute::With takes is the actual mapped route that will call the route function. Remember to have the preceding '/' for most routes. Tokens like {.*} will be matched to whichever type they line up to in the templated argument list. For instance:

~~~~~~~
CreateRoute<int, std::string, float>::With(this, "/books/{id}/{name}/{price}")
~~~~~~~

when matched against

"/books/23/romeoandjuliet/4.99"

will effectively call the function as '(*this)(ctx, 23, "romeoandjuliet", 4.99)'. 

The first argument is the connection context, which is required and passed in automatically. The rest are extracted from the requested URL. 

NOTE: The names within the brackets are optional, and for documentation purposes for the programmer only. They do not need to match up with the name of the arguments in the function themselves, nor should you expect them to be treated as a mapping. It will attempt to apply the arguments in the order you defined them and the order they were encountered in in the route. While it would be great for this automatic mapping to happen, C++ simply does not allow for that level of reflection. 

That being said, it is a good practice to keep the names matched up for readability. 

\subsubsection routefunc Route Function Arguments 

You can specify as many arguments into CreateRoute as you want, and they can be any type that you want. However, it isn't always completely obvious what function declaration they require. 

The first argument is always a reference to a ConnContext object. This is the object you need to write to return information to the client, and so it makes sense for it to be a default. The arguments that follow are the ones specified by CreateRoute, with one caveat: non fundamental types AddPreprocessors to are passed in as const references for performance reasons. 

So from the example above, where we had

~~~~~~~
CreateRoute<int, std::string, float>::With(this, "/books/{id}/{name}/{price}")
~~~~~~~

it is going to want a function defined as such:

void operator()(rikitiki::ConnContext&, int, const std::string&, float)

Under the hood, the routing engine uses the '>>' stream operator to read in the tokens. So out of the box, anything datatype that can be extracted with the '>>' operator. Do note that a specialized case exists for string, in which case it only pulls out a single word and not the rest of the line, as '>>' does by default. 

If you require a custom structure to be mapped, create an appropriate '>>' overload for it and a default constructor. If overloading '>>' is not a viable solution for you, there is also a more specific specialization you can make on 'rikitiki::extract' that is only used for routing. Consult the source and feel free to post to the message board if you have any trouble with this. 

\section conncontext Connection Context Object
The 'ConnContext' object provides all the information about the request, as well as being the object to manipulate. 

The main way to populate the response is to use the stream operator '<<' into the ConnContext object. It supports all the data types that streams accept, as well as 'ContentType' and ctemplate dictionaries. 


\subsection conncontextstreamops Special ConnContext Stream Overloads

As well as normal outputs, you can also use the stream operator to pass in:

- Content types (See rikitiki/content_types.h for a list). You can manually set the content type to any string you want, but the standard ones are defined for you. 
- HttpStatus (See rikitiki/http_statuses.h for a list). It is automatically set to OK. 
    - NOTE: The constructor for HttpStatus is private since you are really recommended to use one of the standard statuses. I'd be curious to know why anyone would need a custom one, and will help add support for that use case if it presents itself. The Response just stores a pointer to a staticly initialized const HttpStatus, so there are definite object life concerns in creating ad-hoc status objects. 
- Header Objects -- See headersTest example. In short, just do 'ctx << rikitiki::Header("Name", "Value")
- Cookie Objects -- See headersTest example. In short, just do 'ctx << rikitiki::Cookie("Name", "Value"). Support for cookie attributes further than that (ie, expires, domain, etc) is forthcoming. 
- CTemplateDictionary -- See the CTemplate section below. Not present if compiled without ctemplate support. 

\section servers Supported Servers 

\subsection Mongoose Mongoose 
Mongoose support is provided via a thin wrapper around native mongoose functionality. A simple example that registers a module and runs mongoose port 5000:

~~~~~~~
int main(){
  MongooseServer server(5000);
  AdvancedModule module;
  server.Register(module);
  server.Start();
  while(true){
    sleep(1000);
  }
  return 0;
}
~~~~~~~

There is no limit to how many modules you can register. 

\subsection apache Apache 

Running the module in apache is more involved, but not by much. A macro does most of the code set-up for us:

~~~~~~
RegisterApacheHandler(helloWorld, HelloWorldModule);
~~~~~~

when compiled to a shared object, this will expose a 'helloWorld_module' to apache. Supposing we compiled a 'mod_helloWorld.so' shared object, we need to take that file and copy or link it to apaches modules directory. On most linux systems, that is going to be /etc/httpd/modules. 

Then we need to add the following line to the httpd.conf file (usually at /etc/httpd/conf/httpd.conf on linux systems):

~~~~~~
LoadModule helloWorld_module modules/mod_helloWorld.so
~~~~~~

or 

~~~~~~
LoadModule helloWorld_module modules/mod_helloWorld.dll
~~~~~~

for windows. 

\section ctemplates CTemplate Integration 
rikitiki comes packaged with [ctemplate](http://code.google.com/p/ctemplate/) support by default, and it is useful as a template engine to separate the presentation layer from the logic layer. 

Use of ctemplates is optional, and you can disable the entire thing by setting 'USE_CTEMPLATE' to 0 in the root CMakeLists.txt file. 

\subsection conncontextctemplate Working with ConnContext and CTemplates 
When compiling with ctemplate support, an overloaded operator is provided for ConnContext and ctemplate::TemplateDictionary which will expand that template into the response. The stream operator is equivalent to calling:

~~~~~~
ctemplate::ExpandTemplate(temlateDictionary.name(), ctemplate::DO_NOT_STRIP, &td, &ctx.response.response);
~~~~~~

For more information on ctemplate, see their [website](http://code.google.com/p/ctemplate/)

\subsection config Config

ctemplate currently only has one setting in the config file: 'ctemplate_root'. This defines the root directory that ctemplate will look for templates in. 

\subsection templatepreprocessors TemplatePreprocessors

If you want to define things like headers and footers, or include site-wide content such as dynamic menus, you can define template preprocessors that will manipulate a dictionary right before it is expanded into the response. A default preprocessor is provided that expands headers, footers, page headers, and page footers. 

Preprocessors are added to the server module with 'server.AddPreprocessor'. This should be done in the register function for a given module. 

Look at the 'advanced' example for an implementation of this feature. 

\section examples Examples

- rikitiki::examples::HelloWorldModule - A simple, one page example which shows the basic idea behind routing.
- rikitiki::examples::AdvancedModule - A more advanced example which shows how to use
    - CTemplates
    - CTemplate preprocessors
    - GET/POST routes
    - Retrieving post and querystring data. 
- rikitiki::examples::HeadersTestModule - Displays the headers and cookies of the page.
- rikitiki::examples::RestModule - Example of RESTful service
 */
