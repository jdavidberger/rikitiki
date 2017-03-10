#include <rikitiki/rikitiki>
#include <assert.h>
#include <memory>
#pragma warning (disable: 4512 )
#include <QUnit.hpp>
#pragma warning (default: 4512 )
#include <mxcomp/vsostream.h>
#include <algorithm>
#include <rikitiki/http/outgoing/Request.h>

namespace rikitiki {
     namespace examples {
          static std::stringstream test_results;
          static std::atomic_int numTests;

          QUnit::UnitTest qunit(test_results, QUnit::verbose);
          std::vector<std::future<void>> active_tests; 

          struct TestsModule FINAL : public WebModule {
               virtual ~TestsModule() {}

               // Make sure we can roundtrip out a payload
               static void BasicTest(std::shared_ptr<Response> response) {
                    std::string payload(response->Body().str());
                    QUNIT_IS_EQUAL(payload, "Basic Test!");
               }
               void BasicTest(ConnContextRef ctx) {
                    ctx << "Basic Test!";
               }
               
               // Make sure whatever ingests route does a good job of it. 
               void QueryStringTest(ConnContextRef ctx, int num) {
                    ctx << "Saw: " << num << " and " << ctx->Request.QueryString()[RT_STRING_LITERAL"qs"];
               }
               static void QueryStringTest(std::shared_ptr<Response> response) {
                    std::string payload(response->Body().str());

                    QUNIT_IS_EQUAL(payload, "Saw: 42 and 43");
               }

               // Make sure we can set a handler. 
               static void HeadersTest(std::shared_ptr<Response> response) {
                    QUNIT_IS_TRUE(response->Headers().size() > 0);
                    auto value = response->Headers().Get(RT_STRING_LITERAL"Test");
                    bool found = value != 0;
                    if (found) {
                         QUNIT_IS_EQUAL(*value, rikitiki::string(RT_STRING_LITERAL"42"));
                    }
                    
                    auto list = response->Headers().GetList(RT_STRING_LITERAL"A");
                    QUNIT_IS_TRUE(list.size() == 3);
                    if (list.size() == 3) {
                         QUNIT_IS_TRUE(list[0] == RT_STRING_LITERAL"a");
                         QUNIT_IS_TRUE(list[1] == RT_STRING_LITERAL"b");
                         QUNIT_IS_TRUE(list[2] == RT_STRING_LITERAL"c");
                    }

                    QUNIT_IS_TRUE(found);
               }
               void HeadersTest(ConnContextRef ctx) {
                    ctx << rikitiki::Header(RT_STRING_LITERAL"A", RT_STRING_LITERAL"a");
                    ctx << rikitiki::Header(RT_STRING_LITERAL"Test1", RT_STRING_LITERAL"42");
                    ctx << rikitiki::Header(RT_STRING_LITERAL"Test", RT_STRING_LITERAL"42");
                    ctx << rikitiki::Header(RT_STRING_LITERAL"A", RT_STRING_LITERAL"b");
                    ctx << rikitiki::Header(RT_STRING_LITERAL"A", RT_STRING_LITERAL"c") << "!";
               }

               // Make sure we can send a payload with the request
               static void PayloadTest(std::shared_ptr<Response> response) {
                    QUNIT_IS_EQUAL("tset a si siht", response->Body().str());
               }
               void PayloadTest(ConnContextRef ctx) {
                    auto str = ctx->Request.Body().str();
                    std::reverse(str.begin(), str.end());
                    ctx << str;
               }


               // Makes ure that the use case where we run async and respond later is handled. 
               static void AsyncTests(std::shared_ptr<Response> response) {
                    std::string payload(response->Body().str());
                    QUNIT_IS_EQUAL(payload, "Testing");
               }
               void AsyncTests(ConnContextRef ctx) {
		 std::async(std::launch::async, [](ConnContextRef ctx) {
                         std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                         ctx << "Testing"; 
                    }, ctx);
               }

               // Do cookie test -- need some more framework stuff. 
               static void CookiesTest(std::shared_ptr<Response> response) {
                    
               }
               void CookiesTest(ConnContextRef ctx) {
                    ctx << rikitiki::Cookie(RT_STRING_LITERAL"Cookie", RT_STRING_LITERAL"12345");
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
                    ctx << "<a href='StartTests'>Start tests</a><br>";
                    for (auto handler : ctx->server->handlers) {                         
		      ctx << "<a href='StartTests?Test=" << handler->name() << "'>" << handler->name() << "</a><br>";
                    }
                
		    std::async(std::launch::async, []( ConnContextRef ctx) {			
			LOG(Tests, Debug) << "Waiting for tests to finish..." << std::endl;
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
               void SetupTest(rikitiki::Server& server, const rikitiki::string& url, void(*testf)(std::shared_ptr<Response>), const std::string& payload = "") {
                    ORequestMemory request;
                    request.uri = RT_STRING_LITERAL"/" + url;
                    if (payload.size()) {
                         request.Body().write(payload.data(), (std::streamsize)payload.size());
                         request.SetContentLength(payload.size());
                         request.SetRequestMethod(RequestMethod::POST);
                    }
                    else {
                         request.SetRequestMethod(RequestMethod::GET);
                    }
                    numTests++;
                    LOG(Tests, Info) << url << " Started." << std::endl;
                    std::shared_future<std::shared_ptr<Response>> future = server.ProcessRequest(request);
                    active_tests.push_back(std::async(std::launch::async, [=] {
                         testf(future.get());
                         LOG(Tests, Info) << url << " done." << std::endl;
                         numTests--;
                    }));                    
               }

               // Run all known tests, then go try to get the status page (Will likely take a second to get results back, an they wont be ordered)
               void StartTests(ConnContextRef ctx) {
                    numTests = 0;
                    auto& server = *ctx->server;
                    test_results.str("");
                    auto test = ctx->Request.QueryString()[RT_STRING_LITERAL"Test"];
                    
                    if (test.empty() || test == RT_STRING_LITERAL"PayloadTest")
                         SetupTest(server, RT_STRING_LITERAL"PayloadTest", &TestsModule::PayloadTest, "this is a test");

                    if (test.empty() || test == RT_STRING_LITERAL"BasicTest")
                         SetupTest(server, RT_STRING_LITERAL"BasicTest", &TestsModule::BasicTest);

                    if (test.empty() || test == RT_STRING_LITERAL"QueryStringTest")
                         SetupTest(server, RT_STRING_LITERAL"QueryStringTest/42?qs=43", &TestsModule::QueryStringTest);

                    if (test.empty() || test == RT_STRING_LITERAL"StatusTest")
                         SetupTest(server, RT_STRING_LITERAL"StatusTest", &TestsModule::StatusTest);

                    if (test.empty() || test == RT_STRING_LITERAL"CookiesTest")
                         SetupTest(server, RT_STRING_LITERAL"CookiesTest", &TestsModule::CookiesTest);

                    if (test.empty() || test == RT_STRING_LITERAL"HeadersTest")
                         SetupTest(server, RT_STRING_LITERAL"HeadersTest", &TestsModule::HeadersTest);

                    if (test.empty() || test == RT_STRING_LITERAL"AsyncTests")
                         SetupTest(server, RT_STRING_LITERAL"AsyncTests", &TestsModule::AsyncTests);
                    
                    this->operator()(ctx);
               }

               void Register(rikitiki::Server& server) OVERRIDE{
                    server.AddHandler(CreateRoute<>::With(this, RT_STRING_LITERAL"/BasicTest", &TestsModule::BasicTest));
                    server.AddHandler(CreateRoute<int>::With(this, RT_STRING_LITERAL"/QueryStringTest/{num}", &TestsModule::QueryStringTest));
                    server.AddHandler(CreateRoute<>::With(this, RT_STRING_LITERAL"/HeadersTest", &TestsModule::HeadersTest));
                    server.AddHandler(CreateRoute<>::With(this, RT_STRING_LITERAL"/PayloadTest", &TestsModule::PayloadTest));
                    server.AddHandler(CreateRoute<>::With(this, RT_STRING_LITERAL"/StatusTest", &TestsModule::StatusTest));
                    server.AddHandler(CreateRoute<>::With(this, RT_STRING_LITERAL"/CookiesTest", &TestsModule::CookiesTest));
                    server.AddHandler(CreateRoute<>::With(this, RT_STRING_LITERAL"/AsyncTests", &TestsModule::AsyncTests));
                    server.AddHandler(CreateRoute<>::With(this, RT_STRING_LITERAL"/StartTests", &TestsModule::StartTests));
                    server.AddHandler(CreateRoute<>::With(this, RT_STRING_LITERAL"/"));

                    server.AddHandler(new StaticContentHandler("/static/", "c:\\"));
               }

          };
     }
}
