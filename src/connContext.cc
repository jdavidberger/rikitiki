/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include "connContext.h"

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

std::ostream& Response::operator <<(const ctemplate::TemplateDictionary& td){   
  ctemplate::ExpandTemplate(td.name(), ctemplate::DO_NOT_STRIP, &td, &response);
  return response;
}

std::ostream& Response::operator ()(const std::string& fn, 
				    const ctemplate::TemplateDictionary& td){    
  ctemplate::ExpandTemplate(fn, ctemplate::DO_NOT_STRIP, &td, &response);
  return response;
}


std::ostream& ConnContext::operator <<(ctemplate::TemplateDictionary& td){   
  handled = true;
  if(server)
    foreach(it, server->templatePreprocessors)
      (*it)->Process(*this, td);
  return response << td;
}

ConnContext::ConnContext(const Server* _server) : handled(false),_method(ANY), mappedPost(false), server(_server) {}

ConnContext::ConnContext() : handled(false),_method(ANY), mappedPost(false), server(NULL) {}
