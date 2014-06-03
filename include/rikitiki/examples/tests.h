#include <rikitiki/rikitiki>
#include <rikitiki\socket.h>
#include <assert.h>
#include <memory>
#pragma warning (disable: 4512 )
#include <qunit.hpp>
#pragma warning (default: 4512 )
#include <mxcomp/vsostream.h>
namespace rikitiki {
     namespace examples {
          QUnit::UnitTest qunit(mxcomp::vsdb, QUnit::verbose);
          void Test(std::shared_ptr<Response> response) {
               std::string payload(response->response.str());
               QUNIT_IS_EQUAL(payload, "Basic Test!");
          }

          void QueryStringTest(std::shared_ptr<Response> response) {
               std::string payload(response->response.str());

               QUNIT_IS_EQUAL(payload, "Saw: 42");
          }

          void HeaderTest(std::shared_ptr<Response> response) {
               QUNIT_IS_TRUE(response->headers.size() > 0);
               bool found = false;
               for (auto hd : response->headers) {
                    if (hd.first == L"Test") {
                         found = true;
                         QUNIT_IS_EQUAL(hd.second, std::wstring(L"42"));
                    }                    
               }
               QUNIT_IS_TRUE(found);
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

               void CookieTest(ConnContextRef ctx) {
                    ctx << rikitiki::Cookie(L"Cookie", L"12345");
               }

               void StatusTest(ConnContextRef ctx) {
                    ctx << rikitiki::HttpStatus::Moved_Permanently;
               }

               void Register(rikitiki::Server& server) OVERRIDE{
                    server.AddHandler(CreateRoute<>::With(this, L"/basictest", &TestsModule::BasicTest));
                    server.AddHandler(CreateRoute<int>::With(this, L"/querystringtest/{num}", &TestsModule::QueryStringTest));
                    server.AddHandler(CreateRoute<>::With(this, L"/headertest", &TestsModule::HeaderTest));

                    SimpleRequest request;
                    request.uri = L"basictest";
                    request.method = IRequest::GET;
                    {
                         std::shared_future<std::shared_ptr<Response>> future = server.ProcessRequest(request);
                         std::async([=] { Test(future.get()); });
                    }
                    
                    request.uri = L"querystringtest/42";
                    request.method = IRequest::GET;
                    {
                         std::shared_future<std::shared_ptr<Response>> future = server.ProcessRequest(request);
                         std::async([=] { examples::QueryStringTest(future.get()); });
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