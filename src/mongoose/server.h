/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include "../server.h"

#ifdef _Win32
#include <Windows.h>
#define Sleep usleep
#else
#include <unistd.h>
#endif

class MongooseServer : public Server {
  mg_context *ctx;
 public:
  MongooseServer(int _port);
  void Start();
  void Stop();
};
