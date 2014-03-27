#pragma once 

#include <string>
#include <vector>
#include <stdint.h>
#include <rikitiki\connContext.h>

namespace rikitiki {
     namespace websocket {
          namespace OpCode {
               enum Data {
                    Text = 0x01,
                    Binary = 0x02
               };
               enum Control {
                    None = 0x00,
                    Close = 0x80,
                    Ping = 0x90,
                    Pong = 0xA0
               };
          }

          struct FrameInfo {
               unsigned char opcode;
               uint64_t length;
               bool isMasked;
               bool isFinished;
               uint32_t mask;

               OpCode::Data DataOpCode() const;
               OpCode::Control ControlOpCode() const;
          };

          struct Frame {
               FrameInfo info;
               const unsigned char* data;
               void Init(unsigned char, const unsigned char* data, size_t);
               Frame(const unsigned char*, size_t);
               Frame(unsigned char, const unsigned char* data, size_t);
          };

          struct Message {
               OpCode::Data opCode;
               std::vector<unsigned char> data;
               size_t frameLength;
               void Ingest(const Frame& frame);
               Message(const Frame& frame);
          };

          const static size_t MAX_FRAME = 10;
          struct WebsocketContext : public virtual RequestContext {
          private:
               virtual int raw_write(const unsigned char*, size_t) = 0;

          public:
               std::vector<std::string> availableProtocols;

               std::string key;
               std::string SelectedProtocol;
               unsigned Version;

               virtual void Ping();
               virtual void Write(const char*, size_t, OpCode::Data opcode = OpCode::Binary);
               virtual void Write(const unsigned char*, size_t, OpCode::Data opcode = OpCode::Binary);
               virtual void Write(const std::string& buffer);

               WebsocketContext();
          };
     }
}
