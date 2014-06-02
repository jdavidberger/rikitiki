#include <rikitiki/rikitiki>
#include <rikitiki\socket.h>
#include <assert.h>
#include <memory>

namespace rikitiki {
	namespace examples {

		void Test(const char* buffer, size_t length) {
			UNREFERENCED_PARAMETER(length);
			std::string payload(buffer);
			
			assert(std::string(buffer) == "Basic Test!");
		}

		struct TestsModule FINAL : public WebModule {
			std::auto_ptr<Socket> testSocket;
			virtual ~TestsModule() {}

			void BasicTest(ConnContextRef ctx) {
				ctx << "Basic test!";
			}
			void Register(rikitiki::Server& server) OVERRIDE {
				server.AddHandler(CreateRoute<>::With(this, L"/basictest", &TestsModule::BasicTest));

				testSocket = server.GetDirectSocket();
				assert(testSocket.get() != 0);
				
				if (testSocket.get() != 0) {
				testSocket->OnData = &Test;
				testSocket->Send("GET /basictest HTTP/1.1\r\n\r\n");
				}
			}

		};
	}
}