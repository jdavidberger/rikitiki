#include <rikitiki/websocket/websocket>
#include <assert.h>
#include <stdint.h>

#include <WinSock2.h>

namespace rikitiki {
	namespace websocket {

		void WebsocketContext::Write(const std::string& buffer) {
			Write((unsigned char*)&buffer[0], buffer.size(), OpCode::Text);
		}
		void WebsocketContext::Ping() {

		}
		WebsocketContext::WebsocketContext(websocket::Server*s) : server(s), Version(0) {
			
		}

		void WebsocketContext::Write(const char* buffer, size_t len, OpCode::T opcode){
			Write((const unsigned char*)buffer, len, opcode);
		}

		void WebsocketContext::Write(const unsigned char* buffer, size_t len, OpCode::T opcode){
			unsigned char header[10];
			uint8_t header_len = 0;
			header[0] = opcode;
			while (len > 0) {
				uint64_t send_size = len > MAX_FRAME ? MAX_FRAME : len;
				len -= send_size;

				OpCode::T control = len == 0 ? OpCode::Final : OpCode::Continuation;
				header[0] |= control;

				if (send_size < 126) {
					header_len = 2; 
					header[1] = send_size;
				}
				else if (send_size < (1 << 16)) {
					header_len = 4; // control, len control, 2 byte length
					header[1] = 126; 
					*((uint16_t*)&header[2]) = htons(send_size);					
				}
				else {
					header_len = 10; // control, len control, 8 byte length
					header[1] = 127;
					*((uint64_t*)&header[2]) = htonll(send_size);
				}
				 
				
				if ((header_len > 0 && raw_write(header, header_len) <= 0) || // These things returning 0 or -1 indicate the socket is closed
					(send_size  > 0 && raw_write(buffer, send_size) <= 0)) {
					this->server->Close(this);
					return;
				}
				
				buffer += send_size;
				header[0] = 0;
			}
		}

		OpCode::T FrameInfo::OpCode() const {
			return (OpCode::T)(opcode & 0xFF);
		}


		void Frame::Init(unsigned char opcode, const unsigned char* buffer, size_t len){
			info.length = len;
			info.isFinished = opcode & OpCode::Close;
			info.opcode = opcode & 0xEF;
			data = buffer;
		}

		Frame::Frame(const unsigned char* buffer, size_t len) {
			unsigned char opcode = buffer[0];
			size_t length = 0;
			buffer++;
			info.isMasked = buffer[0] && 0x80;

			if (buffer[0] == 0x7E) {
				length = *(uint16_t*)(&buffer[1]);
				buffer += sizeof(uint16_t)+1;
			}
			else if (buffer[0] == 0x7F) {
				length = *(uint64_t*)(&buffer[1]);
				buffer += sizeof(uint64_t)+1;
			}
			else {
				length = buffer[0];
				buffer++;
			}

			if (info.isMasked) {
				info.mask = *(uint32_t*)buffer;
				buffer += sizeof(uint32_t);
			}

			Init(opcode, buffer, length);
		}

		Frame::Frame(unsigned char opcode, const unsigned char* buffer, size_t len){
			Init(opcode, buffer, len);
		}

		void Message::Ingest(const Frame& frame) {
			assert(opCode == frame.info.OpCode());
			frameLength++;
			size_t baseSize = data.size();
			if (frame.info.length) {
				data.resize(baseSize + frame.info.length);
				std::memcpy(&data[baseSize], frame.data, frame.info.length);
			}
		}

		Message::Message(const Frame& frame) {
			frameLength = 0;
			opCode = frame.info.OpCode();
			Ingest(frame);
		}
	}
}
