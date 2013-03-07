/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */
#pragma once
#include <rikitiki/server.h>
#include <rikitiki/routing.h>

namespace rikitiki {
  namespace rest {
    template<typename T> int Register(Server& server, const std::string& route, T& t);    
  }
}

#include "rest.tcc"

