/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#pragma once
#include "mongoose/mongoose.h"
//#include "ctemplate/template_emitter_ext.h"
#include <string>
#include "server.h"
#include "http_statuses.h"
namespace rikitiki {
  class Server;
  class ConnContext;

  typedef std::pair<std::string,std::string> stringpair;
  struct Header : public stringpair {
  Header(const std::string& name, const std::string& value) : stringpair(name, value){}
  };
  struct PostContent : public stringpair {
  PostContent(const std::string& name, const std::string& value) : stringpair(name, value){}
  };
  struct Cookie : public stringpair {
  Cookie(const std::string& name, const std::string& value) : stringpair(name, value){}
  };

  struct Response {
    ContentType::t ResponseType;
    
    std::vector<Header> headers;
    const HttpStatus* status;
    std::stringstream response;
    Response();
    template <class T>
    Response& operator <<(const T& obj){ response << obj; return *this;}
    Response& operator <<(rikitiki::ContentType::t t);
    Response& operator <<(const rikitiki::Cookie& t);
    Response& operator <<(const rikitiki::HttpStatus& t);
    Response& operator <<(const rikitiki::Header& t);
  };

  template <typename T1, typename T2> 
    struct multimap : public std::multimap<T1, T2> {
    T2& operator[](const T1&);
  };

  typedef multimap<std::string, std::string> HeaderCollection;
  typedef std::map<std::string, std::string> QueryStringCollection;
  typedef multimap<std::string, std::string> PostCollection;
  typedef std::map<std::string, std::string> CookieCollection;

  class ConnContext {  
  public:
    enum Method {
      ANY = 0, GET = 1, POST = 2, HEAD = 3, PUT = 4, DELETE = 5, TRACE = 6, OPTIONS = 7, CONNECT = 8, PATCH = 9, OTHER
    };
  protected:
    bool mappedPost, mappedQs, mappedHeaders, mappedCookies; 
    PostCollection _post;
    QueryStringCollection _qs;
    HeaderCollection _headers;
    CookieCollection _cookies;

    Method _method;

    virtual void FillQueryString() = 0;
    virtual void FillPost() = 0;
    virtual void FillHeaders() = 0;
    virtual void FillRequestMethod() = 0;  
    virtual void FillCookies();  

    friend class Server;
    virtual void writeResponse() = 0;
    ConnContext(const Server*);
    ConnContext();
  public:
    const Server* server;
    PostCollection& Post();
    QueryStringCollection& QueryString();
    HeaderCollection& Headers();
    CookieCollection& Cookies();

    virtual const char* URI() = 0;
    bool handled;  
    Method RequestMethod();

    template <class T> ConnContext& operator <<(const T& obj);
    Response response;
  };
  void mapContents(std::string& raw_content, PostCollection& post);
  void mapQueryString(const char* _qs, QueryStringCollection& qs);
  ConnContext::Method strToMethod(const char* method);
#include "connContext.tcc"
}



//#include "ctemplate/connContext_ext.h"
