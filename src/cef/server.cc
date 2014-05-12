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
            
            std::shared_ptr<ConnContext> ctx; 
            Server* server; 
       public:
            ResourceHandler(Server* _server, Handler& _handler) : handler(_handler), server(_server) {
                 
            }

            virtual bool ProcessRequest(CefRefPtr<CefRequest> request,
                 CefRefPtr<CefCallback> callback) OVERRIDE{                 
                 ctx = std::shared_ptr<ConnContext>(new ConnContext(server, request));
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
                 response->SetMimeType(ToString(ctx->response.ResponseType));
            }

            virtual bool ReadResponse(void* data_out,
                 int bytes_to_read,
                 int& bytes_read,
                 CefRefPtr<CefCallback> callback) OVERRIDE{
                 
                 ctx->response.response.get((wchar_t*)data_out, bytes_to_read);
                 bytes_read = ctx->response.response.gcount() * sizeof(wchar_t);
                 std::wstring buffer((wchar_t*)data_out, (wchar_t*)((int)data_out + bytes_read));
                 
                 std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
                 auto buffer8 = conv.to_bytes(buffer);
                 memcpy(data_out, &buffer8[0], buffer8.size());
                 bytes_read = buffer8.size();

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

    CefRefPtr<CefResourceHandler> CefInternalServer::GetResourceHandler(CefRefPtr<CefBrowser> browser,
									CefRefPtr<CefFrame> frame,
									CefRefPtr<CefRequest> request) {
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
