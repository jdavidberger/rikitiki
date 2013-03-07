/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */
#pragma once
#include "../server.h"
#include <mongoose.h>
#ifdef _Win32
#include <Windows.h>
#define Sleep usleep
#else
#include <unistd.h>
#endif

namespace rikitiki {
  namespace mongoose {
    /**
       Mongoose wrapper. Contains base server functionality, plus also exposes Start/Stop methods.
     */
    class MongooseServer : public Server {
      mg_context *ctx;
      int port;
    public:
      int Port() { return port; }
      MongooseServer(int _port);
      void Start();
      void Stop();
    };
  }
}
