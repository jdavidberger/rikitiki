/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include <rikitiki/cef/cef>
#include <sstream>
#include <codecvt>
#include <string>

namespace rikitiki {
  namespace cef {
       class ConnContext : public rikitiki::ConnContext, public cef::RequestContext {
       public:
            ConnContext(Server* s, CefRefPtr<CefRequest> r) : rikitiki::ConnContext(s), cef::RequestContext(r) {}
            CefRefPtr<CefCallback> headersReady;
            CefRefPtr<CefCallback> dataReady;
            void Close() {}
            virtual int rawWrite(const char* buffer, size_t length) { return 0; };
            virtual void writeResponse() {};
            virtual void FillPayload() {};
            virtual void OnHeadersFinished() {
                 headersReady->Continue();
            }
            virtual void OnData() {
                 if (dataReady.get() != 0)
                      dataReady->Continue();
            }
       };

       class ResourceHandler : public CefResourceHandler {
            Handler& handler; 
            
            ConnContextRef_<cef::ConnContext> ctx;
            Server* server; 
       public:
            ResourceHandler(Server* _server, Handler& _handler) : handler(_handler), server(_server) {
                 
            }

            virtual bool ProcessRequest(CefRefPtr<CefRequest> request,
                 CefRefPtr<CefCallback> callback) OVERRIDE{                 
                 ctx = ConnContextRef_<cef::ConnContext>(new ConnContext(server, request));
                 ctx->headersReady = callback;
                 handler.Handle(ctx);
                 return true;
            }

            virtual void GetResponseHeaders(CefRefPtr<CefResponse> response,
                 int64& response_length,
                 CefString& redirectUrl) OVERRIDE{

                 response->SetStatus(ctx->response.status->status);
                 response->SetStatusText(ctx->response.status->name);
                 response_length = -1;
                  
                 CefRequest::HeaderMap map;
                 response->GetHeaderMap(map);
                 for (auto it = ctx->response.headers.begin(); it != ctx->response.headers.end(); it++)
                      map.insert(*it);
                 response->SetMimeType(ctx->response.ResponseType);
            }

            virtual bool ReadResponse(void* data_out,
                 int bytes_to_read,
                 int& bytes_read,
                 CefRefPtr<CefCallback> callback) OVERRIDE{
                 ctx->response.response.read((char*)data_out, bytes_to_read);                 
                 bytes_read = ctx->response.response.gcount();

                 ctx->dataReady = bytes_read == 0 ? callback : 0;
                 if (bytes_read)
                      return true;
                 if (ctx.use_count() == 1)
                    return false;
                 return true; 
            }

            virtual void Cancel() OVERRIDE {

            }

            IMPLEMENT_REFCOUNTING(ResourceHandler);
       };

       CefInternalServer::CefInternalServer(const std::wstring& _host) : hostname(_host) {}

    CefRefPtr<CefResourceHandler> CefInternalServer::GetResourceHandler(CefRefPtr<CefBrowser> browser,
									CefRefPtr<CefFrame> frame,
									CefRefPtr<CefRequest> request) {
         if (wcsncmp(request->GetURL().c_str(), hostname.c_str(), hostname.size()) != 0)
              return 0; 
         cef::RequestContext _request(request);
         auto handler = this->GetHandler(_request);
         if (handler) {
              CefRefPtr<CefResourceHandler> rtn(new ResourceHandler(this, *handler));
              return rtn;
         }
         return 0;
    }; 

   }
}
