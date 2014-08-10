/* Copyright (C) 2012-2013 Justin Berger
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include <rikitiki/cef/cef>
#include <rikitiki\socket.h>
#include <sstream>
#include <codecvt>
#include <string>
#include <include\cef_urlrequest.h>

#include "RequestClient.h"
#include "connContext.h"

namespace rikitiki {
     namespace cef {
          class ConnContext : public rikitiki::ConnContext_ < cef::Request, cef::Response, CefRefPtr<CefRequest> > {
          public:

               ConnContext(Server* s, CefRefPtr<CefRequest> r) :
                    rikitiki::ConnContext_<cef::Request, cef::Response, CefRefPtr<CefRequest>>(s, r) {}

          };

          class ResourceHandler : public CefResourceHandler {
               Handler& handler;

               ConnContextRef_<cef::ConnContext> ctx;
               Server* server;
               ResourceHandler& operator = (const ResourceHandler&);
          public:
               ~ResourceHandler() {

               }
               ResourceHandler(Server* _server, Handler& _handler) : handler(_handler), server(_server) {

               }

               virtual bool ProcessRequest(CefRefPtr<CefRequest> request,
                    CefRefPtr<CefCallback> callback) OVERRIDE{

                    ctx = ConnContextRef_<cef::ConnContext>(new ConnContext(server, request));

                    ctx->response.headersReady = callback;
                    handler.Handle(ctx);
                    if (ctx.use_count() == 1 && ctx->response.headersReady) {
                         callback->Continue();
                    }
                    return true;
               }

                    virtual void GetResponseHeaders(CefRefPtr<CefResponse> response,
                    int64& response_length,
                    CefString& redirectUrl) OVERRIDE{
                    UNREFERENCED_PARAMETER(redirectUrl);
                    response->SetStatus(ctx->response.status->status);
                    response->SetStatusText(ctx->response.status->name);
                    response_length = (int64_t)ctx->response.ContentLength();

                    CefRequest::HeaderMap map;
                    response->GetHeaderMap(map);

                    for (auto it = ctx->response.Headers().begin(); it != ctx->response.Headers().end(); it++)
                         map.insert(*it);
                    response->SetHeaderMap(map);
                    response->SetMimeType(ContentType::ToString(ctx->response.ContentType()));
               }

                    virtual bool ReadResponse(void* data_out,
                    int bytes_to_read,
                    int& bytes_read,
                    CefRefPtr<CefCallback> callback) OVERRIDE{
                    std::lock_guard<std::mutex> lock(ctx->response.payloadWrite);

                    ctx->response.Body().read((char*)data_out, bytes_to_read);
                    bytes_read = (int)ctx->response.Body().gcount();
                    ctx->response.Body().clear();
                    ctx->response.dataReady = bytes_read == 0 ? callback : 0;
                    if (bytes_read)
                         return true;
                    if (ctx.use_count() == 1)
                         return false;
                    return true;
               }

                    virtual void Cancel() OVERRIDE{

               }

               IMPLEMENT_REFCOUNTING(ResourceHandler);
          };
#ifdef RT_USE_WEBSOCKET
          websocket::WebsocketProcess* CefInternalServer::HandleWs(websocket::ConnectionHandle) {
               return 0;
          }
#endif
          CefInternalServer::CefInternalServer(const std::wstring& _host) : hostname(_host) {

          }

          CefRefPtr<CefResourceHandler> CefInternalServer::GetResourceHandler(CefRefPtr<CefBrowser> browser,
               CefRefPtr<CefFrame> frame,
               CefRefPtr<CefRequest> request) {
               if (wcsncmp(request->GetURL().c_str(), hostname.c_str(), hostname.size()) != 0)
                    return 0;
               cef::Request _request(request);
               auto handler = this->GetHandler(_request);
               if (handler) {
                    CefRefPtr<CefResourceHandler> rtn(new ResourceHandler(this, *handler));
                    return rtn;
               }
               return 0;
          };

          CefRefPtr<CefResourceHandler> CefInternalServer::Create(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& scheme_name, CefRefPtr<CefRequest> request) {
               UNREFERENCED_PARAMETER(scheme_name);
               // Return a new resource handler instance to handle the request.
               return GetResourceHandler(browser, frame, request);
          }

          std::future<std::shared_ptr<rikitiki::Response>> CefInternalServer::ProcessRequest(rikitiki::Request& _request) {
               auto request = CefRequest::Create();
               request->SetMethod(rikitiki::RequestMethod::ToString((_request.RequestMethod())));
               request->SetURL(hostname + _request.URI());

               auto post = CefPostData::Create();
               auto elem = CefPostDataElement::Create();
               auto req = _request.Body().str();
               
               elem->SetToBytes(req.length(), &req[0]);
               post->AddElement(elem);
               request->SetPostData(post);
               auto pClient = new RequestClient();

               CefRefPtr<CefURLRequestClient> client = pClient;

               // Start the request. MyRequestClient callbacks will be executed asynchronously.
               CefRefPtr<CefURLRequest> url_request = CefURLRequest::Create(request, client);
               return pClient->promise.get_future();
          }
     }
}
