/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */
#include <rikitiki/server.h>
#include <rikitiki/apache/register.h>
#include "${imodule}.h"

void registerServer(rikitiki::Server* server) {
  rikitiki::examples::${modstruct} ${module};
  server->Register(${module});
}

RT_APACHE_MODULE( ${module}, registerServer )
