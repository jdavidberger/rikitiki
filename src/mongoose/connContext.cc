/* Copyright (C) 2012-2013 Justin Berger
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include <rikitiki/mongoose/connContext.h>
#include <cstring>
#include <mxcomp/log.h>
#include <mxcomp/utf.h>
#include <locale>

#include <mongoose.h>
#include <future>

namespace rikitiki {
     namespace mongoose {
       static inline std::wstring toWString(const char* str) {
	 return mxcomp::utf::convert(str);
       }


          MongooseRequest::~MongooseRequest() {
               conn = 0;               
               if (read_thread.joinable())
                    read_thread.join();
          }
          MongooseRequest::MongooseRequest(mg_connection* c, http_message* msg) : conn(c), message(msg) {
              for(size_t i = 0;i < msg->uri.len;i++)
                    uri.push_back(msg->uri.p[i]);

              if(message->query_string.len > 0)
                  qs.FromQueryString( std::string(message->query_string.p, message->query_string.len));


              for (int i = 0; i < 40; i++) {
                  if(message->header_names[i].len == 0)
                      break;

                  headers.Add(std::string(message->header_names[i].p, message->header_names[i].len),
                              std::string(message->header_values[i].p, message->header_values[i].len));
              }

              if(message->body.len > 0)
                  body.str(std::string(message->body.p, message->body.len));
          }
          const wchar_t* MongooseRequest::URI() const {
               return uri.data();
          }

         RequestMethod::t MongooseRequest::RequestMethod() const {
             return RequestMethod::FromString(message->method.p);
         }

         void MongooseRequest::SetRequestMethod(RequestMethod::t t) {
             assert(false);
         }

         QueryStringCollection &MongooseRequest::QueryString() {
             return qs;
         }

         HeaderCollection &MongooseRequest::Headers() {
             return headers;
         }

         ByteStream &MongooseRequest::Body() {
             return body;
         }

         CookieCollection &MongooseRequest::Cookies() {
             return cookies;
         }

         size_t MongooseResponse::WriteData(const char* buff, size_t size){
             LOG(Mongoose, Debug) << "Writing: " << buff << std::endl;
               mg_send(conn, buff, size);
             return size;
          }
          MongooseResponse::MongooseResponse(mg_connection* c) : conn(c) {}

          MongooseConnContextMembers::MongooseConnContextMembers(mg_connection* c, http_message* msg) :
               _request(c, msg), _response(c) {

          }
          MongooseConnContext::MongooseConnContext(Server* s, mg_connection* c, http_message* msg) :
               MongooseConnContextMembers(c, msg),
               ConnContext(s, _request, _response), conn(c), message(msg) {
	    LOG(Mongoose, Debug) << "ConnContext " << this << " created. " << std::endl;
          }
     }
}
