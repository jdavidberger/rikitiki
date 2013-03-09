/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */
#pragma once
#include <rikitiki/server.h>
#include <rikitiki/routing.h>

/**\page rest RESTful web services
   The routing mechanism in rikitiki lends itself to RESTful services pretty well as it is. However, given that there are 8 seperate commonly used entry points per REST interface, having complicated services could clutter up the registration function and make it obnoxious to add additional entry points as you create them. 
   
   The REST extension provides a convienent mechanism to avoid this issue. rikitiki::rest::Register will take a module, and register all of it's entrypoints against the provided server. You can also provide it with the form of the route with an id, but if you don't "route + "/{id}" is used, which is normally what you'd want. 

   Your RESTful module does not have to inherit from any superclass for this functionality to work, it just has to have appropriately named methods defined. Besides sidestepping the need for a virtual table, this also means we are never registering blank handler functions, which would slow down the web server for every request made. The down side to this is that you need to strictly follow the naming scheme for functions in the RESTful module, or they will not be detected and you may not realize it. 
   
~~~~~~
template<typename T> int Register(Server& server, const std::string& route, T& t);
~~~~~~

   The signatures looked for is a void function named after the request type (GET, POST, PUT, etc) in all caps. It should, of course, take in a ConnContext& object. Optionally, to define one for when a route has an ID in it, a second parameter takes in an int value. 

~~~~~~~
struct RestExample {
    void GET(ConnContext& ctx){ ... }
    void GET(ConnContext& ctx, int id){ ... }
    void POST(ConnContext& ctx, int id){ ... }
    void Register(Server& s){
        assert(rikitiki::rest::Register(s, "/restObject", this) == 3);
    }
};
~~~~~~~


   To avoid the situation in which the user misnames a function, the rest registration function returns the number of functions it found and hooked into. It will also print this to the logging mechanism under 'Rest' with a 'Info' level -- unless none is found, and then it reports as an error.

   \sa rikitiki::examples::RestModule, rikitiki/rest/rest.h
 */


namespace rikitiki {
  namespace rest {
    template<typename T> int Register(Server& server, const std::string& route, T* t);    
    template <typename T> int Register(Server& server, const std::string& route, const std::string& routeWId, T* t);
  }
}


#include "rest.tcc"

