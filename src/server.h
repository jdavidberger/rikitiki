#pragma once 

#include <vector>
#include "mongoose/mongoose.h"
#include <sstream>
#include "content_types.h"
#include "utils/macros.h"
#include <tuple>
#include "utils/tuple_ext.h"
#include "utils/log.h"
#include "template.h"
#include <httpd/httpd.h>

class ConnContext;

struct Handler {
  virtual bool Handle(ConnContext& ctx) = 0;
  virtual std::string name() const = 0;
  virtual std::string desc() const;
};

struct CommandHandler : public Handler {
  std::string command;
  bool handleChildren;
  CommandHandler(std::string _command, std::string _desc="");
  virtual void Process(ConnContext& ctx) = 0;
  virtual bool Handle(ConnContext& ctx);
  virtual std::string name() const;
};

class Server {
 protected:
  int port;
  Server(int port);
 public:
  int Port() { return port; }
  std::vector<Handler*> handlers;
  std::vector<TemplatePreprocessor*> templatePreprocessors;
  void* Handle(enum mg_event event, struct mg_connection *conn);
  typedef bool (*handle_t)(ConnContext& ctx);
 
  void Process(ConnContext& ctx);
  void AddPreprocessor( TemplatePreprocessor*);
  void AddHandler( Handler& handler);
  void AddHandler( Handler* handler);
  void AddHandler( handle_t handler );

  template <typename T>
    void AddHandlers( T& t ){
    t.Register( handlers);
  }
};

