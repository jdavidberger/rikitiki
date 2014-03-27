/* Copyright (C) 2012-2013 Justin Berger
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#pragma once
#include <rikitiki\connContext.h>
#include <mongoose.h>

namespace rikitiki {
     struct Server;

     namespace mongoose {

          class MongooseRequestContext : public virtual RequestContext {
          protected:
               virtual void FillQueryString();
               virtual void FillHeaders();               
          public:
               MongooseRequestContext(const mg_request_info*);

               const mg_request_info* request;
               virtual const char* URI();
          };
          /**
             Connection context for Mongoose servers.
             */
          class MongooseConnContext : public MongooseRequestContext, public  ConnContext {
               mg_connection* conn;

          protected:
               virtual void FillPayload();
               virtual void FillRequestMethod();
               
          public:
               MongooseConnContext(Server* s, mg_connection* c);
               virtual void writeResponse();
               virtual int rawWrite(const char* buffer, size_t length);
               virtual void Close();
          };

     }
}
