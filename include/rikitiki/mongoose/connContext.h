/* Copyright (C) 2012-2013 Justin Berger
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#pragma once
#include <rikitiki/connContext.h>
#include <rikitiki/http/helpers/bufferedMessageTypes.h>
#include <rikitiki/http/incoming/Message.h>
#include <string>
#pragma warning(disable:4265 4355 4062)
#include <thread>
#include <future>
#pragma warning(default:4265 4355 4062)

struct mg_connection;
struct http_message;

namespace rikitiki {
     struct Server;

     namespace mongoose {

          class MongooseRequest : public Request {
               rikitiki::string uri; 
               std::thread read_thread;
              QueryStringCollection qs;
              HeaderCollection headers;
              CookieCollection cookies;
              ByteStream body;
              QueryStringCollection post;
          public:
               mg_connection* conn;
              http_message* message;
               MongooseRequest(mg_connection*, http_message*);
               ~MongooseRequest();

              RequestMethod::t RequestMethod() const override;

              void SetRequestMethod(RequestMethod::t t) override;

              QueryStringCollection &QueryString() override;

              virtual const rikitiki::string::value_type * URI() const OVERRIDE;

              HeaderCollection &Headers() override;

              ByteStream &Body() override;

              QueryStringCollection &Post() override;

              CookieCollection &Cookies() override;
          };

          class MongooseResponse : public virtual OResponseWriter {
               mg_connection* conn;
               using OMessageWriter::WriteData;
              virtual size_t WriteData(const char*, size_t) OVERRIDE;

          public:
              void Done() override;

          public:
               MongooseResponse(mg_connection* c);
          };

          struct MongooseConnContextMembers {
               MongooseConnContextMembers(mg_connection* c, http_message* msg);
               MongooseRequest _request;
               MongooseResponse _response;
          };

          class MongooseConnContext : virtual MongooseConnContextMembers, public virtual ConnContext {
                 mg_connection* conn;
                 http_message* message;
                 public:
                      MongooseConnContext(Server* s, mg_connection* c, http_message* msg);
          };
     }
}
