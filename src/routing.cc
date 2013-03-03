#include "routing.h"

Route::Route(const std::string& _route, ConnContext::Method _method) : route(_route), method(_method) {
  
}

Route::Route(const std::string& _route) : route(_route), method(ConnContext::ANY) {
  
}

std::string Route::name() const {
  return route;
}

RouteHandler::RouteHandler() {

}

std::string RouteHandler::name() const {
  return routes[0]->name();
}

bool RouteHandler::Handle(ConnContext& ctx) {
  foreach(it, routes){
    if( (*it)->Handle(ctx) )
      return true;
  }
  return false;
}
