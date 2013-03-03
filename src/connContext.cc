/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include "connContext.h"
#include <algorithm>
#include <curl/curl.h>

namespace rikitiki { 
  ConnContext::Method ConnContext::RequestMethod() {
    if(_method == ANY){
      this->FillRequestMethod();
      assert(_method != ANY);
    }
    return _method;
  }

  std::map<std::string, std::string>& ConnContext::Post() {
    if(!mappedPost){
      this->FillPost();
      assert(mappedPost);
    }
    return _post;
  }

  Response::Response() : ResponseType(web::ContentType::text_html){}

  std::ostream& Response::operator <<(web::ContentType::t t){   
    ResponseType = t;
    return response;
  }

  ConnContext::ConnContext(const Server* _server) : handled(false),_method(ANY), mappedPost(false), server(_server) {}

  ConnContext::ConnContext() : handled(false),_method(ANY), mappedPost(false), server(NULL) {}

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
