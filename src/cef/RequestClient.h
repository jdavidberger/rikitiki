#pragma once 
#include <rikitiki/http/Response.h>

namespace rikitiki {
     namespace cef {
          class RequestClient : public CefURLRequestClient {
public:
     RequestClient() : response(new rikitiki::Response) {}
     virtual ~RequestClient() {}
     std::promise<std::shared_ptr<rikitiki::Response>> promise;
     std::shared_ptr<rikitiki::Response> response;
     virtual void OnRequestComplete(CefRefPtr< CefURLRequest > request) OVERRIDE{
          UNREFERENCED_PARAMETER(request);

          CefResponse::HeaderMap headers;
          response->status = new HttpStatus(request->GetResponse()->GetStatus(), request->GetResponse()->GetStatusText().ToString());
          request->GetResponse()->GetHeaderMap(headers);
          for (auto it = headers.begin(); it != headers.end(); it++) {
               response->Headers().Add(std::wstring(it->first), std::wstring(it->second));
          }
          promise.set_value(response);
     }
          virtual bool GetAuthCredentials(bool isProxy, const CefString& host, int port, const CefString& realm, const CefString& scheme, CefRefPtr< CefAuthCallback > callback) OVERRIDE{
          UNREFERENCED_PARAMETER(isProxy);
          UNREFERENCED_PARAMETER(host);
          UNREFERENCED_PARAMETER(port);
          UNREFERENCED_PARAMETER(realm);
          UNREFERENCED_PARAMETER(scheme);
          UNREFERENCED_PARAMETER(callback);
          return true;
     }
          virtual void OnDownloadData(CefRefPtr< CefURLRequest > request, const void* data, size_t data_length) OVERRIDE{
          std::string d((const char*)data, data_length);
          *response << d;
          UNREFERENCED_PARAMETER(request);
     }

          virtual void OnDownloadProgress(CefRefPtr< CefURLRequest > request, uint64 current, uint64 total) OVERRIDE{
          UNREFERENCED_PARAMETER(request);
          UNREFERENCED_PARAMETER(current);
          UNREFERENCED_PARAMETER(total);
     }

          virtual void OnUploadProgress(CefRefPtr< CefURLRequest > request, uint64 current, uint64 total) OVERRIDE{
          UNREFERENCED_PARAMETER(request);
          UNREFERENCED_PARAMETER(current);
          UNREFERENCED_PARAMETER(total);
     }
     IMPLEMENT_REFCOUNTING(ReqClient);
};
     }
}