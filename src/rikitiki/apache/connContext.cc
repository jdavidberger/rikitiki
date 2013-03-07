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
#include <assert.h>

namespace rikitiki {
  namespace apache {

    void ApacheConnContext::FillRequestMethod() {
      _method = strToMethod(request->method);
    }    

    void ApacheConnContext::FillQueryString() {
      mapQueryString(request->args, _qs);
      mappedQs = true;
    }
    static int header_add(void* rec, const char*key, const char*value){
      HeaderCollection* _headers = (HeaderCollection *)rec;
     _headers->insert(make_pair(std::string(key), std::string(value)));
     return 1;
    }
    void ApacheConnContext::FillHeaders() {
      apr_table_t *h_table = request->headers_in;
      apr_table_do(header_add, &_headers, h_table, NULL);
      mappedHeaders = true;
    }

    void ApacheConnContext::FillPayload() {
      ap_setup_client_block(request, REQUEST_CHUNKED_DECHUNK);
      //      ap_get_client_block(request, 
      _payload.resize(512);
      int nth = 0;
      do{
	_payload.resize( _payload.size() * 2);
	long inc_size = ap_get_client_block(request, &_payload[nth], _payload.size() - nth);
	if(inc_size == -1){
	  response.reset();
	  *this << "Exception while trying to read apache request payload.";
	  throw HandlerException{};
	}
	nth += inc_size;
      } while(nth == (int)_payload.size());
      _payload.resize(nth);
      mappedPayload = true;			  
    }		  
    /*
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
	_post.insert(PostContent(name, value));
      }

      mappedPost = true;
    }
    */
    void ApacheConnContext::writeResponse(){
      std::string resp = response.response.str();
      std::string responseType = ToString(response.ResponseType);
      request->status = response.status->status;
      ap_set_content_type(request, apr_pstrdup(request->pool, &responseType[0]));
      ap_set_content_length(request, resp.size());

      apr_table_t *h_table = request->headers_out;
      foreach(it, response.headers){
	LOG(Server, Debug) << "Setting header " << it->first << ": " << it->second << std::endl;
	apr_table_addn(h_table, &it->first[0], &it->second[0]);
      }

      if(request->header_only) return;
      ap_rputs(resp.c_str(), request);
    }

    ApacheConnContext::ApacheConnContext(Server* server, request_rec* _r) : request(_r), ConnContext(server) { 
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
  }
}
