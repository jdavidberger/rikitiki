/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */
#pragma once
#include <rikitiki/rikitiki>
#include <mxcomp/useful_macros.h>

struct server_rec;
struct request_rec;

namespace rikitiki {
  namespace apache {
    /**
       Apache server wrapper. Does not currently do much. 
     */
    class ApacheServer : public Server {
      server_rec* server;
    public:
      ApacheServer(server_rec* server);
      bool Handle(request_rec*); 
      virtual std::future<std::shared_ptr<rikitiki::Response> > ProcessRequest(rikitiki::Request&) OVERRIDE;
    };
  }
}
