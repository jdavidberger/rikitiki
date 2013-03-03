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
