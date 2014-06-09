
#include "rikitiki\iis7\server.h"
#include "rikitiki\socket.h"
namespace rikitiki {
     namespace iis7 {
          class II7RequestContext : public virtual RequestContext {
               IHttpRequest* request;
          public:
               II7RequestContext(IHttpRequest* req) : request(req) {

               }

               virtual void FillQueryString() OVERRIDE{
                    mapQueryString(request->GetRawHttpRequest()->CookedUrl.pQueryString, _qs);
                    mappedQs = true;
               }

                    virtual void FillHeaders() OVERRIDE{
                    mappedHeaders = true;
               }
                    virtual void FillRequestMethod() OVERRIDE{
#define VERBSWITCH(V) case HttpVerb ## V: this->_method = V; break;
                    switch (request->GetRawHttpRequest()->Verb) {
                         VERBSWITCH(POST);
                         VERBSWITCH(GET);
                         VERBSWITCH(DELETE);
                         VERBSWITCH(HEAD);
                         VERBSWITCH(PUT);
                         VERBSWITCH(TRACE);
                         VERBSWITCH(CONNECT);
                    case HttpVerbUNLOCK:
                    case HttpVerbMKCOL:
                    case HttpVerbTRACK:
                    case HttpVerbMOVE:
                    case HttpVerbCOPY:
                    case HttpVerbPROPFIND:
                    case HttpVerbPROPPATCH:
                    case HttpVerbLOCK:
                    case HttpVerbSEARCH:
                    case HttpVerbOPTIONS:
                         this->_method = OTHER;
                         break;
                    case HttpVerbInvalid:
                    case HttpVerbUnknown:
                    case HttpVerbUnparsed:

                    case HttpVerbMaximum:
                    default: this->_method = ANY;
                    }
#undef VERBSWITCH

               }
                    virtual const wchar_t* URI() {
                         return request->GetRawHttpRequest()->CookedUrl.pAbsPath;
               }
          };
          class II7ConnContext : public II7RequestContext, public ConnContextWithWrite {
               IHttpContext* iis7ctx;
               bool async; 

          public:
               II7ConnContext(Server* server, IHttpContext* _ctx) : II7RequestContext(_ctx->GetRequest()), 
                                                                      iis7ctx(_ctx), 
                                                                      async(false),
                                                                      ConnContextWithWrite(server) {

               }
               virtual void OnHeadersFinished() OVERRIDE{
                    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

                    for (auto header : response.headers) {
                         std::string name = converter.to_bytes(header.first.c_str());
                         std::string value = converter.to_bytes(header.second.c_str());
                         iis7ctx->GetResponse()->SetHeader(name.c_str(), value.c_str(), (USHORT)value.length(), true);
                    }

                    iis7ctx->GetResponse()->SetStatus((USHORT)response.status->status, response.status->name.c_str());
               }
               void SetAsync() {
                    async = true; 
               }
               virtual void Close() OVERRIDE {
                    ConnContext::Close();
                    rawWrite("", 0);
                    iis7ctx->SetRequestHandled();
                    if (async)
                         iis7ctx->IndicateCompletion(RQ_NOTIFICATION_FINISH_REQUEST);
               }
               virtual void OnData() OVERRIDE{
                    std::stringstream ss;
                    response.response.swap(ss);
                    std::string buffer = ss.str();
                    
                    rawWrite(buffer.c_str(), buffer.length());                    
               }
               virtual size_t rawWrite(const void* buffer, size_t length)  {
                    HTTP_DATA_CHUNK c;

                    c.DataChunkType = HttpDataChunkFromMemory;
                    c.FromMemory.pBuffer = (PVOID)buffer;
                    c.FromMemory.BufferLength = (ULONG)length;
                    DWORD sent; BOOL completed;
                    // Note -- we should make this asyc at some point; figure out memory management detail with data chunk.
                    BOOL moreData = length > 0; 
                    assert(S_OK == iis7ctx->GetResponse()->WriteEntityChunks(&c, 1, false, moreData, &sent, &completed));
                    
                    return sent;
               }

               virtual void FillPayload() OVERRIDE{

               }

          };

          class IIS7Server : public Server, public IHttpModuleFactory, public CHttpModule {
               IHttpModuleRegistrationInfo *   pModuleInfo;
               IHttpServer *                   pHttpServer;
               DWORD                           dwServerVersion;

          public:
               IIS7Server(DWORD _dwServerVersion, IHttpModuleRegistrationInfo * _pModuleInfo, IHttpServer * _pHttpServer) :
                    dwServerVersion(_dwServerVersion), pModuleInfo(_pModuleInfo), pHttpServer(_pHttpServer) {

               }

               virtual HRESULT GetHttpModule(OUT CHttpModule **ppModule, IN IModuleAllocator *) OVERRIDE
               {
                    *ppModule = this;
                    return S_OK;
               }
               
               virtual void Dispose() OVERRIDE{

               }

               virtual void Terminate() OVERRIDE
               {
               }


               std::wstring domain;

               REQUEST_NOTIFICATION_STATUS OnAcquireRequestState(IN IHttpContext * pHttpContext, IN OUT IHttpEventProvider * pProvider) OVERRIDE{
                    UNREFERENCED_PARAMETER(pProvider);
                    
                    REQUEST_NOTIFICATION_STATUS rtn = RQ_NOTIFICATION_CONTINUE;
                    ConnContextRef_<II7ConnContext> ctx(new II7ConnContext(this, pHttpContext));
                    auto handler = this->GetHandler(*ctx.get());
                    if (handler != 0 && (handler->Handle(ctx) == true)) {
                         if (ctx.use_count() == 1) {
                              rtn = RQ_NOTIFICATION_FINISH_REQUEST;
                         }
                         else {
                              ctx->SetAsync();
                              rtn = RQ_NOTIFICATION_PENDING;
                         }
                    }
                    return rtn;
               }

               virtual std::future<std::shared_ptr<Response>> ProcessRequest(IRequest& request) OVERRIDE{
                    auto client = new SimpleRequestClient(L"localhost", 80);                    
                    client->MakeRequest(request);
                    return client->future();
               }
          };
          Server* CreateServer(DWORD dwServerVersion, IHttpModuleRegistrationInfo * pModuleInfo, IHttpServer * pHttpServer) {
               auto rtn = new IIS7Server(dwServerVersion, pModuleInfo, pHttpServer);
               pModuleInfo->SetRequestNotifications(rtn, RQ_ACQUIRE_REQUEST_STATE, 0);
               return rtn;
          }

     }
}