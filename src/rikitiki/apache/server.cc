/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include "server.h"
#include <sstream>

namespace rikitiki {
  namespace apache {
    ApacheServer::ApacheServer(server_rec* _server) : server(_server)  {  
    }
  }
}
