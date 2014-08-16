/* Copyright (C) 2012-2013 Justin Berger
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#pragma once
#include <rikitiki/connContext.h>
#include <rikitiki/http/helpers/bufferedMessageTypes.h>
#include <rikitiki/http/incoming/Message.h>
#include <mongoose.h>
#include <string>
#pragma warning(disable:4265 4355 4062)
#include <thread>
#include <future>
#pragma warning(default:4265 4355 4062)

namespace rikitiki {
     struct Server;

     namespace mongoose {

          class MongooseRequest : public IMessage_<http::helpers::BufferedRequest> {               
               std::wstring uri; 
               std::thread read_thread;                
          public:
               mg_connection* conn;
               MongooseRequest(mg_connection*);
               ~MongooseRequest();
               virtual const wchar_t* URI() const OVERRIDE;
               virtual void FillHeaders(HeaderCollection&) const OVERRIDE;
               virtual void FillRequestMethod(Request::Method&) const OVERRIDE;
               virtual void FillQueryString(QueryStringCollection&) const OVERRIDE;
          };

          class MongooseResponse : public virtual OResponseWriter {
               mg_connection* conn;
               using OMessageWriter::WriteData;
               virtual size_t WriteData(const char*, size_t) OVERRIDE;
          public:
               MongooseResponse(mg_connection* c);
          };

          struct MongooseConnContextMembers {
               MongooseConnContextMembers(mg_connection* c);
               MongooseRequest _request;
               MongooseResponse _response;
          };

          class MongooseConnContext : virtual MongooseConnContextMembers, public virtual ConnContext {
                 mg_connection* conn;               
                 public:
                      MongooseConnContext(Server* s, mg_connection* c);
          };
     }
}
