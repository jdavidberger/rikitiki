/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include "../server.h"
#include <httpd/httpd.h>

namespace rikitiki {
  namespace apache {
    class ApacheServer : public Server {
    public:
      server_rec* server;
      ApacheServer(server_rec* server);
    };
  }
}
