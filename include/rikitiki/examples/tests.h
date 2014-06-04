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
          std::stringstream test_results;
          std::atomic_int numTests;

          QUnit::UnitTest qunit(test_results, QUnit::verbose);
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
               static void makeheader(ConnContextRef ctx) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
                    ctx << rikitiki::Header(L"Test", L"42") << "!";
               }
               void HeaderTest(ConnContextRef ctx) {
                    ctx << rikitiki::Header(L"Test1", L"42");
                    std::async([](ConnContextRef ctx) {
                         std::this_thread::sleep_for(std::chrono::milliseconds(5000));
                         ctx << rikitiki::Header(L"Test", L"42") << "!";
                    }, ctx);
               }

               void CookieTest(ConnContextRef ctx) {

                    ctx << rikitiki::Cookie(L"Cookie", L"12345");
               }

               void StatusTest(ConnContextRef ctx) {
                    ctx << rikitiki::HttpStatus::Moved_Permanently;
               }

               void operator () (ConnContextRef ctx) {
                    std::async([](ConnContextRef ctx) {
                         ctx << "<div>";
                         // Note -- this is a race condition prone kind of thing....
                         while (numTests != 0) {
                              std::this_thread::sleep_for(std::chrono::milliseconds(100));
                             ctx << ".";
                         }
                         ctx << "</div>";
                         ctx << "<pre>" << test_results.str() << "</pre>";
                    }, ctx);
               }
               void SetupRouteAndTest() {

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
                    
                    server.AddHandler(CreateRoute<>::With(this, L"/"));
                    server.AddHandler(CreateRoute<>::With(this, L"/basictest", &TestsModule::BasicTest));
                    server.AddHandler(CreateRoute<int>::With(this, L"/querystringtest/{num}", &TestsModule::QueryStringTest));
                    server.AddHandler(CreateRoute<>::With(this, L"/headertest", &TestsModule::HeaderTest));

                    //SetupTest(server, L"basictest", &Test);
                    //SetupTest(server, L"querystringtest/42", &examples::QueryStringTest);
                    SetupTest(server, L"headertest", &examples::HeaderTest);

               }

          };
     }
}