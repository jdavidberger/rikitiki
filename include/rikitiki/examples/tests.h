#include <rikitiki/rikitiki>
#include <rikitiki\socket.h>
#include <assert.h>
#include <memory>


namespace rikitiki {
     namespace examples {

          void Test(std::shared_ptr<Response> response) {
               std::string payload(response->response.str());

               assert(payload == "Basic Test!");
          }

          void QueryStringTest(std::shared_ptr<Response> response) {
               std::string payload(response->response.str());

               assert(payload == "Saw: 42");
          }

          void HeaderTest(std::shared_ptr<Response> response) {
               assert(response->headers[0].first == std::wstring(L"Test") &&
                    response->headers[0].second == std::wstring(L"123"));                              
          }
          struct TestsModule FINAL : public WebModule {
               std::auto_ptr<Socket> testSocket;
               virtual ~TestsModule() {}

               void BasicTest(ConnContextRef ctx) {
                    ctx << "Basic Test!";
               }
               void QueryStringTest(ConnContextRef ctx, int num) {
                    ctx << "Saw: " << num;
               }
               void HeaderTest(ConnContextRef ctx) {
                    ctx << rikitiki::Header(L"Test", L"42") << "!";
               }
               void Register(rikitiki::Server& server) OVERRIDE{
                    server.AddHandler(CreateRoute<>::With(this, L"/basictest", &TestsModule::BasicTest));
                    server.AddHandler(CreateRoute<int>::With(this, L"/querystringtest/{num}", &TestsModule::QueryStringTest));
                    server.AddHandler(CreateRoute<>::With(this, L"/headertest", &TestsModule::HeaderTest));

                    SimpleRequest request;
                    request.uri = L"basictest";
                    request.method = IRequest::GET;
                    {
                         //std::shared_future<std::shared_ptr<Response>> future = server.ProcessRequest(request);
                         //std::async([=] { Test(future.get()); });
                    }
                    
                    request.uri = L"querystringtest/42";
                    request.method = IRequest::GET;
                    {
                         //std::shared_future<std::shared_ptr<Response>> future = server.ProcessRequest(request);
                         //std::async([=] { examples::QueryStringTest(future.get()); });
                    }

                    request.uri = L"headertest";
                    request.method = IRequest::GET;
                    {
                         std::shared_future<std::shared_ptr<Response>> future = server.ProcessRequest(request);
                         std::async([=] { examples::HeaderTest(future.get()); });
                    }
               }

          };
     }
}