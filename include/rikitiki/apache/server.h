/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */
#pragma once
#include <rikitiki/rikitiki>
#include <httpd/httpd.h>

namespace rikitiki {
  namespace apache {
    /**
       Apache server wrapper. Does not currently do much. 
     */
    class ApacheServer : public Server {
    public:
      server_rec* server;
      ApacheServer(server_rec* server);
    };
  }
}
