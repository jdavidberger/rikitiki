/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include "connContext.h"
#include <curl/curl.h>
#include "apr_strings.h"
#include <httpd/httpd.h>
#include <httpd/http_config.h>
#include <httpd/http_protocol.h>
#include <httpd/ap_config.h>
#include <apr_tables.h>

namespace rikitiki {
  namespace apache {

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
	
      ap_set_content_type  (request, apr_pstrdup(request->pool, &responseType[0]));
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
      apr_array_header_t* p_data;
      int res = ap_parse_form_data(request, NULL, &p_data, -1, HUGE_STRING_LEN);      
      if(res != OK || !p_data) assert(false);
      apr_off_t len;
      apr_size_t size;
      std::string name, value;
      while(p_data && !apr_is_empty_array(p_data)){
	ap_form_pair_t *pair = (ap_form_pair_t*) apr_array_pop(p_data);

	apr_brigade_length(pair->value, 1, &len);	
	size = (apr_size_t) len;	
	value.resize(size);
        apr_brigade_flatten(pair->value, &value[0], &size);

	name = std::string(pair->name);
	_post[name] = value;
      }

      mappedPost = true;
    }
  }
}
