
#include <rikitiki\iis7\server.h>
#include <rikitiki\http\helpers\SimpleRequestClient.h>
#include <rikitiki\http\helpers\bufferedMessageTypes.h>
#include <codecvt>

namespace rikitiki {
     namespace iis7 {
          class II7Request : public IMessage_ < rikitiki::http::helpers::BufferedRequest > {
               IHttpRequest* request;
          public:
               II7Request(IHttpContext& httpCtx) {
                    request = httpCtx.GetRequest();
               }
               II7Request(IHttpRequest* req) : request(req) {

               }

               virtual void FillQueryString(QueryStringCollection& qs) const OVERRIDE{
                    qs.FromQueryString(request->GetRawHttpRequest()->CookedUrl.pQueryString);
               }

               virtual void FillHeaders(HeaderCollection&) const OVERRIDE{
                    
                    assert(false);
               }
               
               virtual void FillRequestMethod(Request::Method& m) const {
#define VERBSWITCH(V) case HttpVerb ## V: m = RequestMethod::V; break;
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
                         m = RequestMethod::OTHER;
                         break;
                    case HttpVerbInvalid:
                    case HttpVerbUnknown:
                    case HttpVerbUnparsed:

                    case HttpVerbMaximum:
                    default: m = RequestMethod::ANY;
                    }
#undef VERBSWITCH

               }
               virtual const wchar_t* URI() const {
                    return request->GetRawHttpRequest()->CookedUrl.pAbsPath;
               }
          };
          class II7Response : public OResponse {
               IHttpContext& httpCtx; 
          public:
               II7Response(IHttpContext& _httpCtx) : httpCtx(_httpCtx) {}
               virtual II7Response& operator <<(const rikitiki::HttpStatus& t) OVERRIDE {
                    httpCtx.GetResponse()->SetStatus((USHORT)t.status, t.name.data());
                    return *this;
               }
               virtual void WriteHeader(const Header& header) {
                    rikitiki::string_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

                    std::string name = converter.to_bytes(header.first.data());
                    std::string value = converter.to_bytes(header.second.data());
                    httpCtx.GetResponse()->SetHeader(name.data(), value.data(), (USHORT)value.length(), true);
               }

               virtual size_t WritePayloadData(const char* buffer, size_t length) {
                    HTTP_DATA_CHUNK c;

                    c.DataChunkType = HttpDataChunkFromMemory;
                    c.FromMemory.pBuffer = (PVOID)buffer;
                    c.FromMemory.BufferLength = (ULONG)length;
                    DWORD sent; BOOL completed;
                    // Note -- we should make this asyc at some point; figure out memory management detail with data chunk.
                    BOOL moreData = length > 0;
                    assert(S_OK == httpCtx.GetResponse()->WriteEntityChunks(&c, 1, false, moreData, &sent, &completed));

                    return sent;
               }
          };

          class II7ConnContext : public ConnContext_ < II7Request, II7Response, IHttpContext > {

               IHttpContext* iis7ctx;
               bool async;

          public:
               II7ConnContext(Server* server, IHttpContext* _ctx) : ConnContext_ < II7Request, II7Response, IHttpContext >(server, *_ctx),
                    iis7ctx(_ctx),
                    async(false) {

               }

               void SetAsync() {
                    async = true;
               }
               virtual void Close() OVERRIDE{
                    ConnContext::Close();
                    iis7ctx->SetRequestHandled();
                    if (async)
                         iis7ctx->IndicateCompletion(RQ_NOTIFICATION_FINISH_REQUEST);
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

               virtual
                    REQUEST_NOTIFICATION_STATUS
                    OnBeginRequest(
                    _In_ IHttpContext *         pHttpContext,
                    _In_ IHttpEventProvider *   pProvider
                    ) OVERRIDE{
                    UNREFERENCED_PARAMETER(pProvider);
                    UNREFERENCED_PARAMETER(pHttpContext);

                    REQUEST_NOTIFICATION_STATUS rtn = RQ_NOTIFICATION_CONTINUE;

                    ConnContextRef_<II7ConnContext> ctx(new II7ConnContext(this, pHttpContext));

                    auto handler = this->GetHandler(ctx->Request);
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

                    virtual
                    REQUEST_NOTIFICATION_STATUS
                    OnPostBeginRequest(
                    _In_ IHttpContext *         pHttpContext,
                    _In_ IHttpEventProvider *   pProvider
                    )
               {
                    UNREFERENCED_PARAMETER(pHttpContext);
                    UNREFERENCED_PARAMETER(pProvider);

                    return RQ_NOTIFICATION_CONTINUE;
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


               rikitiki::string domain;

               virtual std::future<std::shared_ptr<Response>> ProcessRequest(Request& request) OVERRIDE{
                    auto client = new SimpleRequestClient(L"localhost", 80);
                    client->MakeRequest(request);
                    return client->future();
               }
          };

          Server* CreateServer(DWORD dwServerVersion, IHttpModuleRegistrationInfo * pModuleInfo, IHttpServer * pHttpServer) {
               auto rtn = new IIS7Server(dwServerVersion, pModuleInfo, pHttpServer);
               pModuleInfo->SetRequestNotifications(rtn, RQ_BEGIN_REQUEST, RQ_BEGIN_REQUEST);
               return rtn;
          }

     }
}
