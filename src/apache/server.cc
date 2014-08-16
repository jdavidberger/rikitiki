/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include <rikitiki/apache/apache>
#include <rikitiki/http/incoming/Message.h>
#include <rikitiki/http/parsing/BufferedReader.h>
#include <rikitiki/http/helpers/bufferedMessageTypes.h>
#include <sstream>

namespace rikitiki {
  namespace apache {
    class ApacheRequest : public IMessage_ <rikitiki::http::helpers::BufferedRequest > {
      request_rec& request;
    public:
      ApacheRequest(request_rec& req) : request(req) {

      }

      virtual void FillQueryString(QueryStringCollection& qs) const OVERRIDE{
	assert(false);
      }

      virtual void FillHeaders(HeaderCollection&) const OVERRIDE{
	assert(false);
      }
               
      virtual void FillRequestMethod(Request::Method& m) const {
	assert(false);
      }
      virtual const wchar_t* URI() const {
	assert(false);
      }
    };

    class ApacheOResponse : public rikitiki::OResponse {
      request_rec& request;
    public:
      ApacheOResponse(request_rec& rec) : request(rec) {}

      virtual ApacheOResponse& operator <<(const rikitiki::HttpStatus& t) OVERRIDE {
	assert(false);
	return *this;
      }
      virtual void WriteHeader(const Header& header) {
	assert(false);
      }

      virtual size_t WritePayloadData(const char* buffer, size_t length) {
	assert(false);
      }
    };

    struct ConnContext : public ConnContext_<ApacheRequest, ApacheOResponse, request_rec > {
      typedef ConnContext_<ApacheRequest, ApacheOResponse, request_rec> CC;
      ConnContext(Server* server, request_rec* _ctx) : CC(server, *_ctx) {
	
      }
    };


    ApacheServer::ApacheServer(server_rec* _server) : server(_server)  {  
    }    
  
    bool ApacheServer::Handle(request_rec* req) {
      ConnContextRef_<ConnContext> ctx(new ConnContext(this, req));
      return Server::Handle(ctx);
    }
  }
}
