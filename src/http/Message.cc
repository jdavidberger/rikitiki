#include <mxcomp/log.h>
#include <rikitiki/http/Header.h>
#include <rikitiki/http/Message.h>
#include <locale>
#include <algorithm>
#include <codecvt>
#include <assert.h>

namespace rikitiki {
     Message& Message::operator <<(const rikitiki::Header& header){          
          Headers().Add(header.first, header.second);
          return *this;
     }

     Message& Message::operator <<(rikitiki::ContentType::t t){
          SetContentType(t);
          return *this;
     }

     Message& Message::operator <<(const rikitiki::Cookie& cookie){
          auto header = Header(L"Set-Cookie", cookie.first + L"=" + cookie.second);
          return *this << header;
     }

     size_t Message::ContentLength() const {          
          return Headers().Get<size_t>(L"Content-Length", (size_t)-1);
     }

     Encoding::t Message::TransferEncoding() const {
          return Headers().Get<Encoding::t>(L"Transfer-Encoding", Encoding::UNKNOWN);
     }

     ContentType::t Message::ContentType() const {
          return Headers().Get<ContentType::t>(L"contenttype", ContentType::text_html);
     }

     void Message::SetContentLength(size_t l) {
          Headers().Set(L"Content-Length", l);          
     }
     void Message::SetTransferEncoding(Encoding::t enc){
          Headers().Set(L"Transfer-Encoding", Encoding::ToString(enc));          
     }
     void Message::SetContentType(ContentType::t t){
          Headers().Set(L"Content-Type", ContentType::ToString(t));
     }

     std::ostream& operator<< (std::ostream& stream, const Message& m){
          std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
          stream << converter.to_bytes(m.Startline()) << std::endl;
          for (auto header : m.Headers()){
               stream << converter.to_bytes(header.first) << ": " << 
                    converter.to_bytes(header.second) << std::endl;
          }
          stream << std::endl << std::endl;
          stream << m.Body().str();
          
          return stream;
     }
     std::wostream& operator<< (std::wostream& stream, const Message& m){
          std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
          stream << m.Startline() << std::endl;
          for (auto header : m.Headers()){
               stream << header.first << ": " << header.second << std::endl;
          }
          stream << std::endl << std::endl;
          stream << converter.from_bytes( m.Body().str() );

          return stream;
     }

     InvalidStateException::InvalidStateException(const std::string& msg) : std::exception(msg.data()) {

     }
     OMessage& OMessage::operator << (rikitiki::ContentType::t t) {
          this->WriteHeader(Header(L"Content-Type", ContentType::ToString(t)));
          return *this;
     }
     OMessage& OMessage::operator <<(const rikitiki::Cookie& t)  {
          *this << Header(L"Set-Cookie", t.first + L"=" + t.second);
          return *this;
     }
     OMessage& OMessage::operator <<(const rikitiki::Header& t)  {
          this->WriteHeader(t);
          return *this;
     }
     void OMessage::SetState(MessageState::type st) {
          OnStateChange(st);
          streamState.streamState = st;
     }
     size_t OMessage::WritePayloadData(const char*, size_t size){
          switch (streamState.streamState) {
          case MessageState::START_LINE:
               SetState(MessageState::HEADERS);
          case MessageState::HEADERS:
               streamState.bodyType = BodyType::From(contentSize, encoding);
               if (streamState.bodyType == BodyType::NONE)
                    streamState.bodyType = BodyType::BUFFERING;
               SetState(MessageState::BODY);
          case MessageState::BODY:          
               return size;          
          case MessageState::FINISHED:
          default:
               throw new InvalidStateException("Could not write header");
          }
     }

     size_t OMessageWriter::WritePayloadData(const char* buffer, size_t size){
          auto oldStreamState = streamState.streamState;
          if (streamState.streamState == MessageState::START_LINE)
               WriteStartLine();

          OMessage::WritePayloadData(buffer, size);

          if (oldStreamState <= MessageState::HEADERS && streamState.bodyType != BodyType::BUFFERING)
               WriteData("\r\n");

          switch (streamState.bodyType) {
          case BodyType::BUFFERING:
               this->buffer.append(buffer, size);
               return size;
          case BodyType::CHUNKED: {
               std::stringstream ss;
               ss << std::hex << size << "\r\n";
               return WriteData(ss.str()) +
                    WriteData(buffer, size) +
                    WriteData("\r\n");
          }
          case BodyType::KNOWN_SIZE:
               return WriteData(buffer, size);
          case BodyType::UNKNOWN:
               throw std::exception("Should not get here.");
          }
          throw std::exception("Should not get here.");
     }
     size_t OMessage::WritePayloadData(const std::string& buff) {
          return WritePayloadData(buff.data(), buff.length());
     }


     void OMessageWriter::Done() {
          switch (streamState.streamState) {
          case MessageState::START_LINE:
               WriteStartLine();               
          case MessageState::HEADERS:
               if (contentSize == -1 && encoding != Encoding::chunked) {
                    WriteHeader(Header(L"Content-Length", L"0"));
                    WriteData("\r\n");
                    return;
               }
               else if (encoding == Encoding::chunked) {
                    WriteData("\r\n0\r\n\r\n");
                    return;
               }
               WriteData("\r\n");
               if (contentSize != 0) {
                    throw std::exception("Connection was abandoned prematurely");
               }
               return;
          case MessageState::BODY:
               switch (streamState.bodyType) {
               case BodyType::CHUNKED:
                    WriteData("0\r\n\r\n");
                    break;
               case BodyType::BUFFERING: {
                    std::wstringstream ss; ss << buffer.size();
                    streamState = MessageState::HEADERS;
                    WriteHeader(Header(L"Content-Length", ss.str()));
                    WriteData("\r\n");
                    WriteData(buffer);
                    break;
               }
               case BodyType::KNOWN_SIZE:
               case BodyType::UNKNOWN: 
                    throw InvalidStateException("");
               }
               break;
          case MessageState::FINISHED:
               return;
          }
     }
     void OMessageWriter::WriteHeader(const Header& header) {
          switch (streamState.streamState) {
          case MessageState::START_LINE:
               WriteStartLine();
               SetState(MessageState::HEADERS);
          case MessageState::HEADERS: {
               WriteData(header.first);
               WriteData(": ");
               WriteData(header.second);
               WriteData("\r\n");
               return;
          }
          default:
               throw new InvalidStateException("Could not write header");
          }
     }

     size_t OMessageWriter::WriteData(const wchar_t* buffer, size_t size) {
          std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
          auto strBuffer = converter.to_bytes(buffer, buffer + size);
          return WriteData(strBuffer.data(), strBuffer.length());
     }
     size_t OMessageWriter::WriteData(const std::wstring& str) {
          return WriteData(str.data(), str.length());
     }
     size_t OMessageWriter::WriteData(const std::string& str) {
          return WriteData(str.data(), str.length());
     }

     const HeaderCollection& Message::Headers() const {
          HeaderCollection& rtn = const_cast<Message*>(this)->Headers();
          return rtn;
     }

     const ByteStream& Message::Body() const {
          ByteStream& rtn = const_cast<Message*>(this)->Body();
          return rtn;
     }

     const CookieCollection& Message::Cookies() const {
          CookieCollection& rtn = const_cast<Message*>(this)->Cookies();
          return rtn;
     }

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
                    SetState(ContentLength() == 0 ? MessageState::FINISHED : MessageState::BODY);
                    bufferMode = TransferEncoding() == Encoding::chunked ? NEWLINE : NONE;
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
                              currentState = MessageState::FINISHED;
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
                              LOG(Message, Warning) << "Data in message was longer than expected.";
                         }
                    }

                    if (expectedSize == 0)
                         currentState.streamState = MessageState::FINISHED;
                    break;
               case BodyType::BUFFERING:
               case BodyType::UNKNOWN:
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