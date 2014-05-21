#include "rikitiki\iis7\server.h"

namespace rikitiki {
     namespace iis7 {
          class II7ConnContext : public ConnContext {
               IHttpContext* iis7ctx; 
               public:
                    II7ConnContext(IHttpContext* _ctx) : iis7ctx(_ctx) {

                    }
                    virtual void FillQueryString() OVERRIDE{

                    }
                    virtual void FillPayload() OVERRIDE{

                    }
                    virtual void Close() OVERRIDE{

                    }
                    virtual void FillHeaders() OVERRIDE{

                    }
                    virtual void FillRequestMethod() OVERRIDE{
#define VERBSWITCH(V) case HttpVerb ## V: this->_method = V; break;
                         switch (iis7ctx->GetRequest()->GetRawHttpRequest()->Verb) {
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
                         return iis7ctx->GetRequest()->GetRawHttpRequest()->CookedUrl.pFullUrl;
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

               REQUEST_NOTIFICATION_STATUS OnAcquireRequestState(IN IHttpContext * pHttpContext, IN OUT IHttpEventProvider * pProvider) OVERRIDE {
                    auto pt = pHttpContext;                    
                    ConnContextRef_<II7ConnContext> ctx(new II7ConnContext(pt));
                    auto handler = this->GetHandler(*ctx.get());
                    if (handler == 0)
                         return RQ_NOTIFICATION_FINISH_REQUEST;
                    if(handler->Handle(ctx) == false)
                         return RQ_NOTIFICATION_FINISH_REQUEST;
                    if (ctx.use_count() == 1)
                         return RQ_NOTIFICATION_FINISH_REQUEST;
                    return RQ_NOTIFICATION_PENDING;
               }
          };
          Server* CreateIIS7Server(DWORD dwServerVersion, IHttpModuleRegistrationInfo * pModuleInfo, IHttpServer * pHttpServer) {
               auto rtn = new IIS7Server(dwServerVersion,  pModuleInfo, pHttpServer);
               pModuleInfo->SetRequestNotifications(rtn, RQ_ACQUIRE_REQUEST_STATE, 0);
               return rtn;
          }
          
     }
}