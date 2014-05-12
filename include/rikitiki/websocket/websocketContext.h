#pragma once 

#include <string>
#include <vector>
#include <stdint.h>
#include <rikitiki\connContext.h>
#include <rikitiki\websocket\websocketServer.h>

namespace rikitiki {
	namespace websocket {
		namespace OpCode {
			// https://tools.ietf.org/html/rfc6455#page-29
			enum T : uint8_t {
				Continuation = 0x00,
				Text = 0x01,
				Binary = 0x02,
				Close = 0x8,
				Ping = 0x9,
				Pong = 0xA,
				Final = 0x80
			};
		}

		struct FrameInfo {
			unsigned char opcode;
			uint64_t length;
			bool isMasked;
			bool isFinished;
			uint32_t mask;

			OpCode::T OpCode() const;			
		};

		struct Frame {
			FrameInfo info;
			const unsigned char* data;
			void Init(unsigned char, const unsigned char* data, size_t);
			Frame(const unsigned char*, size_t);
			Frame(unsigned char, const unsigned char* data, size_t);
		};

		struct Message {
			OpCode::T opCode;
			std::vector<unsigned char> data;
			size_t frameLength;
			void Ingest(const Frame& frame);
			Message(const Frame& frame);
		};

		const static size_t MAX_FRAME = 4294967296;
		struct WebsocketContext : public virtual RequestContext {
		private:
			virtual int raw_write(const unsigned char*, size_t) = 0;

		public:
			rikitiki::websocket::Server* server;
			std::vector<std::string> availableProtocols;

			std::string key;
			std::string SelectedProtocol;
			unsigned Version;

			virtual void Ping();
			virtual void Write(const char*, size_t, OpCode::T opcode = OpCode::Binary);
			virtual void Write(const unsigned char*, size_t, OpCode::T opcode = OpCode::Binary);
			virtual void Write(const std::string& buffer);

			WebsocketContext(websocket::Server*);
		};
	}
}
