#include "connContext.h"
#include <curl/curl.h>
#include <algorithm>

void MongooseConnContext::FillRequestMethod() {
    if(strcmp(request.request_method, "POST") == 0){
      _method = POST;
    } else if(strcmp(request.request_method, "GET") == 0){
      _method = GET;
    }
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

void MongooseConnContext::FillPost() {
  std::string p_data;
  p_data.resize(512);
  int nth = 0;
  do{
    p_data.resize( p_data.size() * 2);
    nth += mg_read(conn, &p_data[nth], p_data.size() - nth);
  } while(nth == (int)p_data.size());
  p_data.resize(nth);
  if(p_data.back() != '&')
    p_data.push_back('&');

  std::replace(p_data.begin(), p_data.end(), '+', ' ');
  auto l_it = p_data.begin();

  std::string name, value;
  foreach(it, p_data){
    switch(*it){
    case '=': 
      name = std::string(l_it, it);	  
      l_it = it+1;
      break; 
    case '&': 
      char* value = curl_unescape(&*l_it, it - l_it);
      
      _post[name] = value;
      curl_free(value);
      l_it = it+1;
      break;
    }
  }
  mappedPost = true;
}

