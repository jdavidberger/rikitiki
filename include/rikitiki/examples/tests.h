#include <rikitiki/rikitiki>
#include <rikitiki\socket.h>
#include <assert.h>
#include <memory>
#pragma warning (disable: 4512 )
#include <qunit.hpp>
#pragma warning (default: 4512 )
#include <mxcomp/vsostream.h>
#include <mxcomp/threadedstringbuf.h>

namespace rikitiki {
     namespace examples {
          static std::stringstream test_results;
          static std::atomic_int numTests;

          QUnit::UnitTest qunit(test_results, QUnit::verbose);
          
          struct TestsModule FINAL : public WebModule {
          
               std::auto_ptr<Socket> testSocket;
               virtual ~TestsModule() {}

               static void BasicTest(std::shared_ptr<Response> response) {
                    std::string payload(response->response.str());
                    QUNIT_IS_EQUAL(payload, "Basic Test!");
               }
               void BasicTest(ConnContextRef ctx) {
                    ctx << "Basic Test!";
               }
               
               void QueryStringTest(ConnContextRef ctx, int num) {
                    ctx << "Saw: " << num;
               }
               static void QueryStringTest(std::shared_ptr<Response> response) {
                    std::string payload(response->response.str());

                    QUNIT_IS_EQUAL(payload, "Saw: 42");
               }

               static void HeadersTest(std::shared_ptr<Response> response) {
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
               void HeadersTest(ConnContextRef ctx) {
                    ctx << rikitiki::Header(L"Test1", L"42");
                    std::async([](ConnContextRef ctx) {
                         ctx << rikitiki::Header(L"Test", L"42") << "!";
                    }, ctx);
               }

               static void CookiesTest(std::shared_ptr<Response> response) {
                    
               }
               void CookiesTest(ConnContextRef ctx) {
                    ctx << rikitiki::Cookie(L"Cookie", L"12345");
               }

               static void StatusTest(std::shared_ptr<Response> response) {
                    QUNIT_IS_EQUAL(rikitiki::HttpStatus::Moved_Permanently.status, response->status->status);
               }
               void StatusTest(ConnContextRef ctx) {
                    ctx << rikitiki::HttpStatus::Moved_Permanently;
               }

               void operator () (ConnContextRef ctx) {
                    ctx << "<html><body>";
                    std::async([](ConnContextRef ctx) {
                         ctx << "<div>";
                         while (numTests != 0) {
                              std::this_thread::sleep_for(std::chrono::milliseconds(100));
                             ctx << ".";
                         }
                         ctx << "</div>";
                         qunit.printStatus();
                         ctx << "<pre>" << test_results.str() << "</pre>";
                         ctx << "</body></html>";
                    }, ctx);
               }

               void SetupTest(rikitiki::Server& server, const std::wstring& url, void(*testf)(std::shared_ptr<Response>)) {
                    SimpleRequest request;
                    request.uri = url;
                    numTests++;
                    request.method = IRequest::GET;
                    {
                         std::shared_future<std::shared_ptr<Response>> future = server.ProcessRequest(request);
                         std::async([=] {
                              testf(future.get());
                              numTests--;
                         });
                    }
               }
               void Register(rikitiki::Server& server) OVERRIDE{
                    numTests = 0;
                    server.AddHandler(CreateRoute<>::With(this, L"/BasicTest", &TestsModule::BasicTest));
                    server.AddHandler(CreateRoute<int>::With(this, L"/QueryStringTest/{num}", &TestsModule::QueryStringTest));
                    server.AddHandler(CreateRoute<>::With(this, L"/HeadersTest", &TestsModule::HeadersTest));
                    server.AddHandler(CreateRoute<>::With(this, L"/StatusTest", &TestsModule::StatusTest));
                    server.AddHandler(CreateRoute<>::With(this, L"/CookiesTest", &TestsModule::CookiesTest));
                    
                    SetupTest(server, L"BasicTest", &TestsModule::BasicTest);
                    SetupTest(server, L"QueryStringTest/42", &TestsModule::QueryStringTest);
                    SetupTest(server, L"StatusTest", &TestsModule::StatusTest);
                    SetupTest(server, L"CookiesTest", &TestsModule::CookiesTest);
                    
                    SetupTest(server, L"HeadersTest", &TestsModule::HeadersTest);

                    server.AddHandler(CreateRoute<>::With(this, L"/"));
               }

          };
     }
}