/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#pragma once
#include "mongoose/mongoose.h"
//#include "ctemplate/template_emitter_ext.h"
#include <string>
#include "server.h"

namespace rikitiki {
  class Server;
  class ConnContext;
  struct Response {
    ContentType::t ResponseType;
    int status;
    std::stringstream response;
    Response();
    template <class T>
    Response& operator <<(const T& obj){ response << obj; return *this;}
    Response& operator <<(rikitiki::ContentType::t t);
  };

  struct Request {
    virtual const char* URI() = 0;
  };

  class ConnContext : public Request {  
  public:
    enum Method {
      ANY = 0, GET = 1, POST = 2, HEAD = 3, PUT = 4, DELETE = 5, TRACE = 6, OPTIONS = 7, CONNECT = 8, PATCH = 9, OTHER
    };
  protected:
    bool mappedPost, mappedQs; 
    std::map<std::string, std::string> _post;
    std::map<std::string, std::string> _qs;

    Method _method;

    virtual void FillQueryString() = 0;
    virtual void FillPost() = 0;
    virtual void FillRequestMethod() = 0;  
    
    friend class Server;
    virtual void writeResponse() = 0;
    ConnContext(const Server*);
    ConnContext();
  public:
    const Server* server;
    std::map<std::string, std::string>& Post();
    std::map<std::string, std::string>& QueryString();

    Method RequestMethod();

    bool handled;  
    template <class T> ConnContext& operator <<(const T& obj);
    Response response;
  };
  void mapContents(std::string& raw_content, std::map<std::string, std::string>& post);
  void mapQueryString(const char* _qs, std::map<std::string, std::string>& qs);
  ConnContext::Method strToMethod(const char* method);
#include "connContext.tcc"
}



//#include "ctemplate/connContext_ext.h"
