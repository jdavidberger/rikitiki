#include <rikitiki/websocket/websocket>
#include <assert.h>
#include <stdint.h>

namespace rikitiki {
  namespace websocket {

    void WebsocketContext::Write(const std::string& buffer) { 
      Write((unsigned char*)&buffer[0], buffer.size(), OpCode::Text); 
    }
    void WebsocketContext::Ping() {
      
    }
    WebsocketContext::WebsocketContext() {
      Version = 0;
    }

    void WebsocketContext::Write(const char* buffer, size_t len, OpCode::Data opcode){
         Write((const unsigned char*)buffer, len, opcode);
    }

    void WebsocketContext::Write(const unsigned char* buffer, size_t len, OpCode::Data opcode){ 
      unsigned char header[2];
	  header[0] = opcode;
      while(len > 0) {
	unsigned char send_size = len > MAX_FRAME ? MAX_FRAME : len;
	len -= send_size;

	OpCode::Control control = len == 0 ? OpCode::Close : OpCode::None;
	header[0] |= control;
	header[1] = send_size;
  
	raw_write(header, 2);
	raw_write(buffer, send_size);
	buffer += send_size;
	header[0] = 0;
      }
    }

    OpCode::Data FrameInfo::DataOpCode() const {
		return (OpCode::Data)(opcode & 0x0F);
    }

    OpCode::Control FrameInfo::ControlOpCode() const {
		return (OpCode::Control)(opcode & 0xF0);
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

      if(buffer[0] == 0x7E) {
	length = *(uint16_t*)(&buffer[1]);
	buffer += sizeof(uint16_t) + 1;
      } else if(buffer[0] == 0x7F) {
	length = *(uint64_t*)(&buffer[1]);
	buffer += sizeof(uint64_t) + 1;
      } else {
	length = buffer[0];
	buffer++;
      }

      if(info.isMasked) {
	info.mask = *(uint32_t*)buffer;
	buffer += sizeof(uint32_t);
      }

      Init(opcode, buffer, length);
    }
    
    Frame::Frame(unsigned char opcode, const unsigned char* buffer, size_t len){
      Init(opcode, buffer, len);
    }

    void Message::Ingest(const Frame& frame) {
      assert(opCode == frame.info.DataOpCode());
      frameLength++;
      size_t baseSize = data.size();
	  if (frame.info.length) {
		data.resize(baseSize + frame.info.length);
		std::memcpy( &data[baseSize], frame.data, frame.info.length);
	  }
    }

    Message::Message(const Frame& frame) {
      frameLength = 0;
      opCode = frame.info.DataOpCode();
      Ingest(frame);
    }
  }
}
