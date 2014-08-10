/* Copyright (C) 2012-2013 Justin Berger
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include <rikitiki/mongoose/connContext.h>
#include <cstring>
#include <mxcomp\log.h>
#include <locale>
#include <codecvt>
#include <mongoose.h>
#include <future>

namespace rikitiki {
     namespace mongoose {
          static inline std::wstring toWString(const char* str) {
               std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> conversion;
               return conversion.from_bytes(str);
          }
          static void read_loop(MongooseRequest* request) {
               char buffer[1024];
               while (request->conn) {                    
                    auto bytes = mg_read(request->conn, buffer, sizeof(buffer));

                    if (request->OnData(buffer, (size_t)bytes))
                         return;
               }
          }

          MongooseRequest::~MongooseRequest() {
               conn = 0;               
               if (read_thread.joinable())
                    read_thread.join();
          }
          MongooseRequest::MongooseRequest(mg_connection* c) : conn(c) {
               auto request = mg_get_request_info(c);
               const char* _uri = request->uri;
               for (; *_uri != 0; _uri++)
                    uri.push_back((wchar_t)*_uri);
               
               currentState.streamState = MessageState::BODY;               
               UpdateBufferState();

               if (currentState.streamState != MessageState::FINISHED)
                    read_thread = std::thread(read_loop, this);
          }
          const wchar_t* MongooseRequest::URI() const {
               return uri.data();
          }
          void MongooseRequest::FillHeaders(HeaderCollection& headers) const {
               auto request = mg_get_request_info(conn);
               for (int i = 0; i < request->num_headers; i++) {
                    headers.Add(request->http_headers[i].name, request->http_headers[i].value);
               }
          };
          
          void MongooseRequest::FillRequestMethod(Request::Method& ) const {
               assert(false);
          }

          size_t MongooseResponse::WriteData(const char* buff, size_t size){
               return (size_t)mg_write(conn, buff, size);
          }
          MongooseResponse::MongooseResponse(mg_connection* c) : conn(c) {}

          MongooseConnContextMembers::MongooseConnContextMembers(mg_connection* c) :
               _request(c), _response(c) {

          }
          MongooseConnContext::MongooseConnContext(Server* s, mg_connection* c) : 
               MongooseConnContextMembers(c),
               ConnContext(s, _request, _response), conn(c) {        

          }

          /*
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
          void MongooseRequestContext::FillPayload() {

          }
          void MongooseConnContext::FillPayload() {
               std::string payload;
               payload.resize(512);
               size_t nth = 0;
               do{                    
                    payload.resize(payload.size() * 2);                            
                    nth += mg_read(conn, &payload[nth], payload.size() - nth);
               } while (nth == (int)payload.size());
               payload.resize(nth);

               _body.write(&payload[0], (std::streamsize)payload.size());
               mappedPayload = true;
          }
          */
     }
}
