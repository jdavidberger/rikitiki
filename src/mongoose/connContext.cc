/* Copyright (C) 2012-2013 Justin Berger
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include <rikitiki/mongoose/connContext.h>
#include <cstring>
#include <mxcomp\log.h>
#include <locale>
#include <codecvt>

namespace rikitiki {
     namespace mongoose {
          static inline std::wstring toWString(const char* str) {
               auto size = strlen(str);
               std::wstring rtn(size, L'\0');
#ifdef _MSC_VER
			   mbstowcs_s(&size, &rtn[0], size, str, size);
#else
               mbstowcs(&rtn[0], str, size);
#endif
               return rtn;
          }


          MongooseRequestContext::MongooseRequestContext(const mg_request_info* _request) : request(_request){
               const char* _uri = request->uri;
               for (; *_uri != 0; _uri++)
                    uri.push_back((wchar_t)*_uri);
          }

          const wchar_t* MongooseRequestContext::URI(){
               return uri.c_str();
          }

          void MongooseRequestContext::FillQueryString() {
               std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
               mapQueryString(converter.from_bytes(request->query_string).c_str(), _qs);
               mappedQs = true;
          }

          void MongooseRequestContext::FillRequestMethod() {
               std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
               _method = strToMethod(converter.from_bytes(request->request_method).c_str());
          }

          void MongooseConnContext::writeResponse(){
               if (conn == 0)
                    return;
               std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;

               std::stringstream ss;
               std::string resp = response.response.str();
               ss << "HTTP/1.1 " << response.status->status << " " << response.status->name << "\r\n";
               ss << "Content-Length: " << resp.size() << "\r\n";
               ss << "Content-Type: " << conv.to_bytes(response.ResponseType) << "\r\n";
               for (auto it : response.headers){
                    ss << conv.to_bytes(it.first) << ": " << conv.to_bytes(it.second) << "\r\n";
               }
               ss << "\r\n";
               ss << resp;
               std::string buffer = ss.str();

               rawWrite(buffer.c_str(), buffer.length());
          }
		  size_t MongooseConnContext::rawWrite(const void* buffer, size_t length){
               return (size_t)mg_write(conn, buffer, length);
          }
          void MongooseConnContext::Close() {
               ConnContextWithWrite::Close();
               conn = 0;
          }
          MongooseConnContext::MongooseConnContext(Server* s, mg_connection* c) :
               RequestContext(),
               ConnContextWithWrite(s),
               MongooseRequestContext(mg_get_request_info(c)),
               conn(c) {

               std::string thread_name = "";
               thread_name += request->uri;
               thread_name += " ";
               thread_name += request->request_method[0];
               thread_name.resize(15);
#ifndef _MSC_VER
               int retval = pthread_setname_np(pthread_self(), thread_name.c_str());
               if(retval != 0)
                    LOG(Mongoose, Error) << "Couldn't set thread name, err: " << retval << std::endl;
#endif
               LOG(Mongoose, Verbose) << "Request for [" << request->request_method << "] " << request->uri << std::endl;
          }


          void MongooseRequestContext::FillHeaders() {

               std::string name, value;
               for (int i = 0; i < request->num_headers; i++)
                    AddRequestHeader(toWString(request->http_headers[i].name), toWString(request->http_headers[i].value));

               mappedHeaders = true;
          }

          void MongooseConnContext::FillPayload() {
               _payload.resize(512);
               size_t nth = 0;
               do{
                    _payload.resize(_payload.size() * 2);
                    nth += mg_read(conn, &_payload[nth], _payload.size() - nth);
               } while (nth == (int)_payload.size());
               _payload.resize(nth);
               mappedPayload = true;
          }

     }
}
