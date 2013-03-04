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

  std::map<std::string, std::string>& ConnContext::QueryString() {
    if(!mappedQs){
      this->FillQueryString();
      assert(mappedQs);
    }
    return _qs;
  }

  std::map<std::string, std::string>& ConnContext::Post() {
    if(!mappedPost){
      this->FillPost();
      assert(mappedPost);
    }
    return _post;
  }

  Response::Response() : ResponseType(rikitiki::ContentType::text_html){}

  Response& Response::operator <<(rikitiki::ContentType::t t){   
    ResponseType = t;
    return *this;
  }

  ConnContext::ConnContext(const Server* _server) : handled(false),_method(ANY), mappedPost(false), server(_server) {}

  ConnContext::ConnContext() : handled(false),_method(ANY), mappedPost(false), server(NULL) {}

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

  void mapContents(std::string& raw_content, std::map<std::string, std::string>& post){
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
      
	post[name] = value;
	curl_free(value);
	l_it = it+1;
	break;
      }
    }
  }
}
