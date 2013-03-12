/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#pragma once
#include <map>
#include <string>
#include "server.h"
#include "http_statuses.h"
#include "metaprogramming.h"
#include "content_handler.h"
namespace rikitiki {
  class Server;
  class ConnContext;

  typedef std::pair<std::string,std::string> stringpair;
  
  /**
     Headers are just string pairs
     We can't just typedef it since we want to pass it around with stream operators
   */
  struct Header : public stringpair {
    Header(const std::string& name, const std::string& value);
  };

  /**
     Things parsed out of the response, ala forms
     We can't just typedef it since we want to pass it around with stream operators
   */
  struct PostContent : public stringpair {
    PostContent(const std::string& name, const std::string& value);
  };

  /**
     Cookies are key value pairs too.
     TODO: Add expiration, domain, etc
   */
  struct Cookie : public stringpair {
    Cookie(const std::string& name, const std::string& value, 
	   const std::string& Domain = "", const std::string& Path = "",
	   const std::string& Expires = "", bool secure = false, bool httpOnly = false);
  };

  /**
     Response class that handlers write to. Contains headers, response stream, status, etc.
   */
  struct Response {
    ContentType::t ResponseType;
    
    std::vector<Header> headers;
    const HttpStatus* status;
    std::stringstream response;
    void reset();
    Response();
    template <class T>

    auto operator <<(const T& obj) -> decltype( instance_of<std::stringstream>::value << obj, instance_of<Response&>::value)
    { response << obj; return *this;}

    Response& operator <<(rikitiki::ContentType::t t);
    Response& operator <<(const rikitiki::Cookie& t);
    Response& operator <<(const rikitiki::HttpStatus& t);
    Response& operator <<(const rikitiki::Header& t);
  };

  /**
     The normal multimap, but with an operator[] defined.
   */
  template <typename T1, typename T2> 
    struct multimap : public std::multimap<T1, T2> {
    T2& operator[](const T1&);
  };

  typedef multimap<std::string, std::string> HeaderCollection;
  typedef std::map<std::string, std::string> QueryStringCollection;
  typedef multimap<std::string, std::string> PostCollection;
  typedef std::map<std::string, std::string> CookieCollection;

  /**
     Connection Context object. This is the main object in which handlers use both to read request data and
     write response data. 

     In general, you can just stream ('<<') whatever you want into it, and there should be an override that 
     does the right thing. 
   */
  class ConnContext {  
  public:
    enum Method {
      ANY = 0, GET = 1, POST = 2, HEAD = 3, PUT = 4, DELETE = 5, TRACE = 6, OPTIONS = 7, CONNECT = 8, PATCH = 9, OTHER
    };
  protected:
    bool mappedPost, mappedQs, mappedHeaders, mappedCookies, mappedPayload, mappedContentType; 
    PostCollection _post;
    QueryStringCollection _qs;
    HeaderCollection _headers;
    CookieCollection _cookies;
    std::multimap<double, ContentType::t>* _accepts;
    ContentType::t _contentType;
    std::string _payload;

    Method _method;

    virtual void FillAccepts();
    virtual void FillContentType();
    virtual void FillPayload() = 0;
    virtual void FillPost();

    virtual void FillQueryString() = 0;
    virtual void FillHeaders() = 0;
    virtual void FillRequestMethod() = 0;  
    virtual void FillCookies();  

    friend class Server;
    virtual void writeResponse() = 0;
    ConnContext(const Server*);
    ConnContext();
  public:
    /**\brief This is a conv. function to add REQUEST headers, not response headers (use the stream operator for that). 
       This function exists so the raw conncontext drivers can just kick down unsanitized header data and this function
       does the right thing. Namely that means lower-casing it. 
    */
    HeaderCollection::value_type& AddRequestHeader(const char*, const char*);
    virtual ~ConnContext();
    const Server* server;
    std::multimap<double, ContentType::t>& Accepts();
    PostCollection& Post();
    ContentType::t ContentType();
    QueryStringCollection& QueryString();
    HeaderCollection& Headers();
    CookieCollection& Cookies();
    std::string& Payload();

    virtual const char* URI() = 0;
    bool handled;  
    Method RequestMethod();

    template <class T> auto operator <<(const T& obj) -> decltype( instance_of<Response>::value << obj, (ConnContext&)*(ConnContext*)0) ;

    template <class T> auto operator <<(T&) -> decltype(valid_conversions<T>::Out::Instance(), instance_of<ConnContext>::value);
    template <class T> auto operator >>(T&) -> decltype(valid_conversions<T>::In ::Instance(), instance_of<ConnContext>::value);
    Response response;
  };
  void mapContents(std::string& raw_content, PostCollection& post);
  void mapQueryString(const char* _qs, QueryStringCollection& qs);
  ConnContext::Method strToMethod(const char* method);

  ConnContext& operator>>(ConnContext&, std::string& t);
#include "connContext.tcc"
}



//#include "ctemplate/connContext_ext.h"
