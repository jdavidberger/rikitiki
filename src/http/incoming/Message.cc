#include <mxcomp\log.h>
#include <rikitiki/http/Header.h>
#include <rikitiki/http/incoming/Message.h>

#include <codecvt>

namespace rikitiki {
     static inline const char* skipWhitespace(const char* data, const char* end) {
          while (data < end &&
               (*data == ' ' || *data == '\t')) {
               data++;
          }
          return data;
     }

     static inline const char* readHeaderName(const char* data, const char* end, std::string& out) {
          out.clear();
          auto br = data;
          while (br < end && !(isspace((int)*br) || *br == ':')) {
               br++;
          }
          out.resize((std::size_t)(br - data)); std::memcpy(&out[0], data, out.size());

          while (br < end && *br != ':') br++;
          while (br < end && *br == ':') br++;

          return skipWhitespace(br, end);
     }
     static inline const char* readHeaderValue(const char* data, const char* end, std::string& out) {
          out.clear();
          auto br = data;
          while (br < end && !(*br == '\r' || *br == '\n')) {
               br++;
          }
          out.resize((std::size_t)(br - data)); std::memcpy(&out[0], data, out.size());
          return br;
     }
     void IMessage::OnStartLine(const std::wstring& startline) {
          this->SetStartline(startline);
     }
     void IMessage::OnHeader(const std::wstring& n, const  std::wstring& v) {
          *this << Header(n, v);
     }
     void IMessage::OnBodyData(const char* data, size_t size) {
          Body().write(data, (std::streamsize)size);
     }
     void IMessage::UpdateBufferState() {
          if (currentState.streamState == MessageState::BODY) {
               currentState.bodyType = BodyType::From(this->ContentLength(), this->TransferEncoding());
               if (currentState.bodyType == BodyType::KNOWN_SIZE) {
                    bufferMode = BufferModeT::LENGTH;
                    expectedSize = ContentLength();
               }
               if (currentState.bodyType == BodyType::CHUNKED)
                    bufferMode = BufferModeT::NEWLINE; // To read in the next chunk size
               if (currentState.bodyType == BodyType::NONE)
                    currentState.streamState = MessageState::FINISHED;
          }
     }

     bool IMessage::OnBufferedData(const char* data, std::size_t length) {
          std::string buffer;
          const char* end = &data[length];
          switch (currentState.streamState) {
          case MessageState::START_LINE: {
               std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> conversion;
               OnStartLine(conversion.from_bytes(data));
               SetState(MessageState::HEADERS);
               return false;
          }
          case MessageState::HEADERS:
               if (data[0] == '\r' || data[0] == '\n') {
                    data += 2;
                    currentState.bodyType = BodyType::From(ContentLength(), TransferEncoding());
                    SetState(ContentLength() == 0 ? MessageState::FINISHED : MessageState::BODY);                    
                    bufferMode = TransferEncoding() == Encoding::chunked ? NEWLINE : NONE;
                    expectedSize = ContentLength();
               }
               else {                    
                    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> conversion;
                    std::string buffer2;
                    data = readHeaderName(data, end, buffer);
                    data = readHeaderValue(data, end, buffer2);                    
                    OnHeader(conversion.from_bytes(&buffer[0]), conversion.from_bytes(&buffer2[0]));
                    data += 2; // Burn \r\n                    
               }
               return currentState.streamState == MessageState::FINISHED;
          case MessageState::BODY: {
               switch (currentState.bodyType) {
               case BodyType::CHUNKED: {
                    if (expectedSize == (size_t)-1) {
                         expectedSize = (size_t)strtol(data, (char**)&data, 16) + 2;
                         bufferMode = LENGTH;
                    }
                    else {
                         auto relevantSize = (std::streamsize)(expectedSize - 2);
                         OnChunkedData(data, (size_t)relevantSize); 
                         if (expectedSize - 2 == 0)
                              currentState.streamState = MessageState::FINISHED;
                         bufferMode = NEWLINE;
                    }
                    break;
               }
               case BodyType::KNOWN_SIZE:
                    if (data != end) {
                         bufferMode = NONE;
                         OnBodyData(data, (size_t)(end - data));
                         if ((size_t)(end - data) <= expectedSize) {
                              expectedSize -= (size_t)(end - data);
                         }
                         else {
                              expectedSize = 0; 
                              LOG(Message, Warning) << "Data in message was longer than expected." << std::endl;
                         }
                    }

                    if (expectedSize == 0)
                         currentState.streamState = MessageState::FINISHED;
                    break;
               case BodyType::BUFFERING:
               case BodyType::UNKNOWN:
                    OnBodyData(data, (size_t)(end - data));
                    break;
               }
               break;
          }
          case MessageState::FINISHED:
               throw new std::exception("Unexpected input");
          }

          return currentState.streamState == MessageState::FINISHED;
     }

}