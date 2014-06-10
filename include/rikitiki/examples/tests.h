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
          std::vector<std::future<void>> active_tests; 

          struct TestsModule FINAL : public WebModule {
          
               std::auto_ptr<Socket> testSocket;
               virtual ~TestsModule() {}

               // Make sure we can roundtrip out a payload
               static void BasicTest(std::shared_ptr<Response> response) {
                    std::string payload(response->payload.str());
                    QUNIT_IS_EQUAL(payload, "Basic Test!");
               }
               void BasicTest(ConnContextRef ctx) {
                    ctx << "Basic Test!";
               }
               
               // Make sure whatever ingests route does a good job of it. 
               void QueryStringTest(ConnContextRef ctx, int num) {
                    ctx << "Saw: " << num;
               }
               static void QueryStringTest(std::shared_ptr<Response> response) {
                    std::string payload(response->payload.str());

                    QUNIT_IS_EQUAL(payload, "Saw: 42");
               }

               // Make sure we can set a handler. 
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
                    ctx << rikitiki::Header(L"Test", L"42") << "!";
               }

               // Makes ure that the use case where we run async and respond later is handled. 
               static void AsyncTests(std::shared_ptr<Response> response) {
                    std::string payload(response->payload.str());
                    QUNIT_IS_EQUAL(payload, "Testing");
               }
               void AsyncTests(ConnContextRef ctx) {
                    std::async([](ConnContextRef ctx) {
                         std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                         ctx << "Testing"; 
                    }, ctx);
               }

               // Do cookie test -- need some more framework stuff. 
               static void CookiesTest(std::shared_ptr<Response> response) {
                    
               }
               void CookiesTest(ConnContextRef ctx) {
                    ctx << rikitiki::Cookie(L"Cookie", L"12345");
               }

               // Make sure we can set the status string however
               static void StatusTest(std::shared_ptr<Response> response) {
                    QUNIT_IS_EQUAL(rikitiki::HttpStatus::Moved_Permanently.status, response->status->status);
               }
               void StatusTest(ConnContextRef ctx) {
                    ctx << rikitiki::HttpStatus::Moved_Permanently;
               }

               // Main page -- show all the tests. Might have to wait on them to finish. 
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

               // Set up a test to be run. 
               void SetupTest(rikitiki::Server& server, const std::wstring& url, void(*testf)(std::shared_ptr<Response>)) {
                    SimpleRequest request;
                    request.uri = url;
                    numTests++;
                    request.method = IRequest::GET;
                    
                    std::shared_future<std::shared_ptr<Response>> future = server.ProcessRequest(request);
                    active_tests.push_back(std::async([=] {
                         testf(future.get());
                         numTests--;
                    }));                    
               }

               // Run all known tests, then go try to get the status page (Will likely take a second to get results back, an they wont be ordered)
               void StartTests(ConnContextRef ctx) {
                    numTests = 0;
                    auto& server = *ctx->server;
                    SetupTest(server, L"BasicTest", &TestsModule::BasicTest);
                    SetupTest(server, L"QueryStringTest/42", &TestsModule::QueryStringTest);
                    SetupTest(server, L"StatusTest", &TestsModule::StatusTest);
                    SetupTest(server, L"CookiesTest", &TestsModule::CookiesTest);
                    SetupTest(server, L"AsyncTests", &TestsModule::AsyncTests);
                    SetupTest(server, L"HeadersTest", &TestsModule::HeadersTest);
                    this->operator()(ctx);
               }

               void Register(rikitiki::Server& server) OVERRIDE{
                    server.AddHandler(CreateRoute<>::With(this, L"/BasicTest", &TestsModule::BasicTest));
                    server.AddHandler(CreateRoute<int>::With(this, L"/QueryStringTest/{num}", &TestsModule::QueryStringTest));
                    server.AddHandler(CreateRoute<>::With(this, L"/HeadersTest", &TestsModule::HeadersTest));
                    server.AddHandler(CreateRoute<>::With(this, L"/StatusTest", &TestsModule::StatusTest));
                    server.AddHandler(CreateRoute<>::With(this, L"/CookiesTest", &TestsModule::CookiesTest));
                    server.AddHandler(CreateRoute<>::With(this, L"/AsyncTests", &TestsModule::AsyncTests));                    
                    server.AddHandler(CreateRoute<>::With(this, L"/StartTests", &TestsModule::StartTests));
                    server.AddHandler(CreateRoute<>::With(this, L"/"));
               }

          };
     }
}