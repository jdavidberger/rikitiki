/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include "connContext.h"
#include <algorithm>
#include <curl/curl.h>
#include <assert.h>

namespace rikitiki { 
  
  ConnContext::Method ConnContext::RequestMethod() {
    if(_method == ANY){
      this->FillRequestMethod();
      assert(_method != ANY);
    }
    return _method;
  }

  static inline const char* read_name(const char* b){
    while(*b == ' ') b++;
    while(*b != '=' && *b != '\0') b++;
    return b;
  }

  static inline const char* read_value(const char* b){
    while(*b != ';' && *b != '\0') b++;
    return b;
  }

  void ConnContext::FillPost(){
    mapContents(Payload(), _post);
    mappedPost = true;
  }

  void ConnContext::FillCookies(){
    auto range = Headers().equal_range("Cookie");
    for(auto it = range.first;it != range.second;it++){
      LOG(Server, Debug) << "Req. cookie: " << it->first << " = " << it->second << std::endl;
      if(it->second.size() == 0) continue;
      const char* n =  &it->second[0];
      const char *ne, *ve;
      do {
	ne = read_name(n);
	ve = read_value(ne);
	_cookies[std::string(n, ne)] = 
	  ne >= ve ? std::string() : std::string(ne+1, ve); 
	n = ve;
	while(*n == ';' || *n == ' ') n++;	
      } while(*n != 0);
    }
    mappedCookies = true;
  }
  
  std::string& ConnContext::Payload() {
    if(!mappedPayload){
      this->FillPayload();
      assert(mappedPayload);
    }
    return _payload;
  }

  HeaderCollection& ConnContext::Headers(){
    if(!mappedHeaders){
      this->FillHeaders();
      assert(mappedHeaders);
    }
    return _headers;
  }

  CookieCollection& ConnContext::Cookies(){
    if(!mappedCookies){
      this->FillCookies();
      assert(mappedCookies);
    }
    return _cookies;
  }

  QueryStringCollection& ConnContext::QueryString() {
    if(!mappedQs){
      this->FillQueryString();
      assert(mappedQs);
    }
    return _qs;
  }

  PostCollection& ConnContext::Post() {
    if(!mappedPost){
      this->FillPost();
      assert(mappedPost);
    }
    return _post;
  }

  Response::Response() : ResponseType(rikitiki::ContentType::DEFAULT), 
			 status(&HttpStatus::OK){}

  Response& Response::operator <<(const rikitiki::HttpStatus& t){
    status = &t;
    return *this;
  }

  Response& Response::operator <<(rikitiki::ContentType::t t){   
    ResponseType = t;
    return *this;
  }

  Response& Response::operator <<(const rikitiki::Cookie& cookie){
    return *this << Header("Set-Cookie", cookie.first + "=" + cookie.second);
  }

  Response& Response::operator <<(const rikitiki::Header& header){   
    headers.push_back(header);
    return *this;
  }

  ConnContext& operator>>(ConnContext& ctx, std::string& t){
    t = ctx.Payload();
    return ctx;
  }

  ConnContext::ConnContext(const Server* _server) : ConnContext() { server = _server; }
  ConnContext::ConnContext() : _method(ANY), 
			       handled(false), 
			       server(NULL), 
			       mappedPost(false), 
			       mappedQs(false), 
			       mappedHeaders(false), 
			       mappedCookies(false), 
			       mappedPayload(false)  {}

#define MATCH_METHOD_ENUM(eval)	do{if(strcmp(method, #eval) == 0) return ConnContext::eval;}while(false);
				     
  ConnContext::Method strToMethod(const char* method){
    MATCH_METHOD_ENUM(GET);
    MATCH_METHOD_ENUM(POST);
    MATCH_METHOD_ENUM(HEAD);
    MATCH_METHOD_ENUM(PUT);
    MATCH_METHOD_ENUM(DELETE);
    MATCH_METHOD_ENUM(TRACE);
    MATCH_METHOD_ENUM(OPTIONS);
    MATCH_METHOD_ENUM(CONNECT);
    MATCH_METHOD_ENUM(PATCH);
    LOG(Server, Error) << "strToMethod failed on method '" << method << "'" << std::endl;
    return ConnContext::ANY;
  }
  void mapQueryString(const char* _qs, std::map<std::string, std::string>& qs){
    if(_qs == NULL) return;
    std::string name;
    std::string* val = 0;
    bool nameMode = true;
    while(*_qs != '\0'){
      if(nameMode){
	if(*_qs == '='){
	  nameMode = false;
	  val = &qs[name];
	} else if(*_qs == '&'){
	  qs[name] = "";
	  name.clear();
	} else {
	  name += *_qs;
	}
      } else {
	if(*_qs == '&'){
	  nameMode = true;
	  name.clear();
	} else {
	  *val += *_qs;
	}
      }
      _qs++;
    }
  }

  void mapContents(std::string& raw_content, PostCollection& post){
    if(raw_content.back() != '&')
      raw_content.push_back('&');
    
    std::replace(raw_content.begin(), raw_content.end(), '+', ' ');
    auto l_it = raw_content.begin();
    std::string name, value;
    foreach(it, raw_content){
      switch(*it){
      case '=': 
	name = std::string(l_it, it);	  
	l_it = it+1;
	break; 
      case '&': 
	char* value = curl_unescape(&*l_it, it - l_it);
	post.insert(PostContent(name, value));
	curl_free(value);
	l_it = it+1;
	break;
      }
    }
  }
}
