/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include "connContext.h"
#include <curl/curl.h>
#include "apr_strings.h"

void ApacheConnContext::FillRequestMethod() {
    if(strcmp(request->method, "POST") == 0){
      _method = POST;
    } else if(strcmp(request->method, "GET") == 0){
      _method = GET;
    }
}

void ApacheConnContext::writeResponse(){
  std::string resp = response.response.str();
  std::string responseType = ToString(response.ResponseType);

  ap_set_content_type  (request, responseType.c_str());
  ap_set_content_length(request, resp.size());

  if(request->header_only)
    return;
  ap_rputs(resp.c_str(), request);
  LOG(Server, Info) << request->status << std::endl;
}


ApacheConnContext::ApacheConnContext(request_rec* _r) : request(_r), ConnContext(NULL) { 
  std::string thread_name = "";
  thread_name += request->uri;
  thread_name += " ";
  thread_name += request->method[0];
  thread_name.resize(15);
  int retval = pthread_setname_np(pthread_self(), thread_name.c_str());
  if(retval != 0)
    LOG(Server, Error) << "Couldn't set thread name, err: " << retval << std::endl;
  LOG(Server, Info) << "Request for [" << request->method << "] " << request->uri << std::endl;
}

const char* ApacheConnContext::URI(){
  return request->uri;
}

void ApacheConnContext::FillPost() {
  mappedPost = true;
}

