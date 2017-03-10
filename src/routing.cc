/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include <rikitiki/routing.h>

namespace rikitiki {
  Route::Route(const rikitiki::string& _route, Request::Method _method) : route(_route), method(_method) {
  
  }

  Route::Route(const rikitiki::string& _route) : route(_route), method(RequestMethod::ANY) {
  
  }

  rikitiki::string Route::name() const {
    return route;
  }
}
