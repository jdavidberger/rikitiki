/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include "connContext.h"
#include <cstring>

namespace rikitiki {
  namespace mongoose {

    void MongooseConnContext::FillRequestMethod() {
      _method = strToMethod(request.request_method);
    }

    void MongooseConnContext::writeResponse(){
      std::stringstream ss;
      std::string resp = response.response.str();
      ss << "HTTP/1.1 200 OK\r\n";
      ss << "Content-Type: " << ToString(response.ResponseType) << "\r\n";
      ss << "Content-Length: " << resp.size() << "\r\n";
      ss << "\r\n";
      ss << resp;
      std::string buffer = ss.str();
      mg_write(conn, buffer.c_str(), buffer.length());  
    }


    MongooseConnContext::MongooseConnContext(const Server* s, 
					     mg_event e, 
					     mg_connection* c) : 
      event(e), 
      conn(c), 
      request(*mg_get_request_info(c)), ConnContext(s) { 

      std::string thread_name = "";
      thread_name += request.uri;
      thread_name += " ";
      thread_name += request.request_method[0];
      thread_name.resize(15);
      int retval = pthread_setname_np(pthread_self(), thread_name.c_str());
      if(retval != 0)
	LOG(Web, Error) << "Couldn't set thread name, err: " << retval << std::endl;
      LOG(Web, Verbose) << "Request for [" << request.request_method << "] " << request.uri << std::endl;
    }

    const char* MongooseConnContext::URI(){
      return request.uri;
    }
    void MongooseConnContext::FillQueryString() {
      mapQueryString(request.query_string, _qs);
      mappedQs = true;
    }
    void MongooseConnContext::FillPost() {
      std::string p_data;
      p_data.resize(512);
      int nth = 0;
      do{
	p_data.resize( p_data.size() * 2);
	nth += mg_read(conn, &p_data[nth], p_data.size() - nth);
      } while(nth == (int)p_data.size());
      p_data.resize(nth);

      mapContents(p_data, _post);

      mappedPost = true;
    }

  }
}
