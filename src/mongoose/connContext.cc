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
          void MongooseRequest::FillQueryString(QueryStringCollection& qs) const {
               auto request = mg_get_request_info(conn);
               if (request->query_string)
                    qs.FromQueryString(request->query_string);
          }
          size_t MongooseResponse::WriteData(const char* buff, size_t size){
               LOG(Mongoose, Debug) << (void*)this << " WriteData: " << std::string(buff, size) << std::endl;
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
     }
}
