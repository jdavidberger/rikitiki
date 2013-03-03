#pragma once
#include "mongoose/mongoose.h"
#include "ctemplate/template_emitter_ext.h"
#include <string>
#include "server.h"

class Server;
class ConnContext;
struct Response {
  web::ContentType::t ResponseType;
  int status;

  std::stringstream response;

  Response();

  template <class T>
  std::ostream& operator <<(const T& obj){ return response << obj; }

  std::ostream& operator <<(web::ContentType::t t);
  std::ostream& operator <<(const ctemplate::TemplateDictionary& td);
  std::ostream& operator ()(const std::string& fn, const ctemplate::TemplateDictionary& td);
};

struct Request {
  virtual const char* URI() = 0;
};

class ConnContext : public Request {  
 public:
  enum Method {
    ANY = 0,
    GET = 1,
    POST = 2,
    OTHER = 3
  };
 private:
 protected:
  bool mappedPost; 
  std::map<std::string, std::string> _post;
  Method _method;

  virtual void FillPost() = 0;
  virtual void FillRequestMethod() = 0;  
  virtual void writeResponse() = 0;
  ConnContext(const Server*);
  ConnContext();
 public:
  const Server* server;
  std::map<std::string, std::string>& Post();

  Method RequestMethod();

  bool handled;  
  template <class T>
  std::ostream& operator <<(const T& obj);

  std::ostream& operator <<(ctemplate::TemplateDictionary& td);
  Response response;
};

#include "connContext.tcc"
