#include "rikitiki\iis7\server.h"

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
                    default: this->_method = ANY;
                    }
#undef VERBSWITCH

               }
                    virtual const wchar_t* URI() {
                         return request->GetRawHttpRequest()->CookedUrl.pFullUrl;
               }
          };
          class II7ConnContext : public II7RequestContext, public ConnContextWithWrite {
               IHttpContext* iis7ctx;
          public:
               II7ConnContext(IHttpContext* _ctx) : II7RequestContext(_ctx->GetRequest()), iis7ctx(_ctx) {

               }
               virtual int rawWrite(const void* buffer, size_t length)  {
                    HTTP_DATA_CHUNK c;
                    c.DataChunkType = HttpDataChunkFromMemory;
                    c.FromMemory.pBuffer = (PVOID)buffer;
                    c.FromMemory.BufferLength = length;
                    DWORD sent; BOOL completed;
                    // Note -- we should make this asyc at some point; figure out memory management detail with data chunk.
                    assert(S_OK == iis7ctx->GetResponse()->WriteEntityChunks(&c, 1, false, true, &sent, &completed));
                    return sent; 
               }
               
               virtual void FillPayload() OVERRIDE{
                    iis7ctx->GetRequest()->GetRawHttpRequest()->pEntityChunks
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

                    virtual void Terminate() OVERRIDE
               {
               }

               REQUEST_NOTIFICATION_STATUS OnAcquireRequestState(IN IHttpContext * pHttpContext, IN OUT IHttpEventProvider * pProvider) OVERRIDE{
                    auto pt = pHttpContext;
                    ConnContextRef_<II7ConnContext> ctx(new II7ConnContext(pt));
                    auto handler = this->GetHandler(*ctx.get());
                    if (handler == 0)
                         return RQ_NOTIFICATION_FINISH_REQUEST;
                    if (handler->Handle(ctx) == false)
                         return RQ_NOTIFICATION_FINISH_REQUEST;
                    if (ctx.use_count() == 1)
                         return RQ_NOTIFICATION_FINISH_REQUEST;
                    return RQ_NOTIFICATION_PENDING;
               }
          };
          Server* CreateServer(DWORD dwServerVersion, IHttpModuleRegistrationInfo * pModuleInfo, IHttpServer * pHttpServer) {
               auto rtn = new IIS7Server(dwServerVersion, pModuleInfo, pHttpServer);
               pModuleInfo->SetRequestNotifications(rtn, RQ_ACQUIRE_REQUEST_STATE, 0);
               return rtn;
          }

     }
}