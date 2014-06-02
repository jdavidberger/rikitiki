/* Copyright (C) 2012-2013 Justin Berger
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include <rikitiki/cef/cef>
#include <rikitiki\socket.h>
#include <sstream>
#include <codecvt>
#include <string>
#include <include\cef_urlrequest.h>

namespace rikitiki {
     namespace cef {
          class ConnContext : public rikitiki::ConnContext, public cef::RequestContext {
          public:
               ConnContext(Server* s, CefRefPtr<CefRequest> r) : rikitiki::ConnContext(s), cef::RequestContext(r) {}
               CefRefPtr<CefCallback> headersReady;
               CefRefPtr<CefCallback> dataReady;
               void Close() {}

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
               ResourceHandler& operator = (const ResourceHandler&);
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
                    UNREFERENCED_PARAMETER(redirectUrl);
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
                    bytes_read = (int)ctx->response.response.gcount();

                    ctx->dataReady = bytes_read == 0 ? callback : 0;
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
          websocket::WebsocketProcess* CefInternalServer::HandleWs(websocket::ConnectionHandle) {
               return 0;
          }
          CefInternalServer::CefInternalServer(const std::wstring& _host) : hostname(_host) {

          }

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

          class ReqClient : public CefURLRequestClient {
          public:
               ReqClient() : response(new Response) {}
               std::promise<std::shared_ptr<Response>> promise;
               std::shared_ptr<Response> response;
               virtual void OnRequestComplete(CefRefPtr< CefURLRequest > request) OVERRIDE {
                    UNREFERENCED_PARAMETER(request);
                    CefResponse::HeaderMap headers; 
                    request->GetResponse()->GetHeaderMap(headers);
                    for (auto it = headers.begin(); it != headers.end(); it++) {
                         response->headers.push_back(Header(std::wstring(it->first), std::wstring(it->second)));
                    }
                    promise.set_value(response);
               }
               virtual bool GetAuthCredentials(bool isProxy, const CefString& host, int port, const CefString& realm, const CefString& scheme, CefRefPtr< CefAuthCallback > callback) OVERRIDE {
                    UNREFERENCED_PARAMETER(isProxy);
                    UNREFERENCED_PARAMETER(host);
                    UNREFERENCED_PARAMETER(port);
                    UNREFERENCED_PARAMETER(realm);
                    UNREFERENCED_PARAMETER(scheme);
                    UNREFERENCED_PARAMETER(callback);
                    return true;
               }
               virtual void OnDownloadData(CefRefPtr< CefURLRequest > request, const void* data, size_t data_length) {
                    std::string d((const char*)data, data_length);
                    *response << d; 
                    UNREFERENCED_PARAMETER(request);                    
               }
                    
               virtual void	OnDownloadProgress(CefRefPtr< CefURLRequest > request, uint64 current, uint64 total) {
                    UNREFERENCED_PARAMETER(request);
                    UNREFERENCED_PARAMETER(current);
                    UNREFERENCED_PARAMETER(total);
               }
                    
               virtual void	OnUploadProgress(CefRefPtr< CefURLRequest > request, uint64 current, uint64 total) {
                    UNREFERENCED_PARAMETER(request);
                    UNREFERENCED_PARAMETER(current);
                    UNREFERENCED_PARAMETER(total);
               }
               IMPLEMENT_REFCOUNTING(ReqClient);
          };

          std::future<std::shared_ptr<Response>> CefInternalServer::ProcessRequest(rikitiki::IRequest& _request) {
               auto request = CefRequest::Create();
               request->SetMethod(rikitiki::methodToStr(_request.RequestMethod()));
               request->SetURL(hostname + _request.URI());
               
               
               CefRefPtr<ReqClient> client = new ReqClient();

               // Start the request. MyRequestClient callbacks will be executed asynchronously.
               CefRefPtr<CefURLRequest> url_request = CefURLRequest::Create(request, client.get());
               return client->promise.get_future();
          }
     }
}
