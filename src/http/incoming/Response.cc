#include <rikitiki/http/Response.h>
#include <assert.h>
#include <locale>
#include <codecvt>

namespace rikitiki {
   static inline const char* skipWhitespace(const char* data, const char* end) {
          while (data < end &&
               (*data == ' ' || *data == '\t')) {
               data++;
          }
          return data;
     }

     static inline const char* skipNonwhitespace(const char* data, const char* end) {
          while (data < end && !isspace((int)*data)) {
               data++;
          }
          return data;
     }

     static inline int expectLinefeed(const char*& data, const char* end) {
          int rtn = 0;
          while (data < end && (*data == '\r' || *data == '\n')) {
               data++;
               rtn++;
          }
          return rtn;
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

     static inline const char* readWord(const char* data, const char* end, std::string& out) {
          out.clear();
          data = skipWhitespace(data, end);
          auto wordEnd = skipNonwhitespace(data, end);
          out.resize((std::size_t)(wordEnd - data)); std::memcpy(&out[0], data, out.size());
          return skipWhitespace(wordEnd, end);
     }

     /*
     BufferedReader::BufferedReader() : bufferMode(NEWLINE), expectedSize((std::size_t) - 1) {}
     bool BufferedReader::OnData(const char* data, std::size_t length) {
          const char* end = &data[length];

          while (data < end) {
               switch (bufferMode) {
               case NONE:
                    return OnBufferedData(data, (size_t)(end - data));
               case NEWLINE: {
                    auto newLinePos = strpbrk(data, "\r\n");
                    if (newLinePos == 0) // can't process yet; kick out
                         goto cant_process;

                    newLinePos++;
                    if (newLinePos[0] == '\r' || newLinePos[0] == '\n')
                         newLinePos++;
                    expectedSize = (size_t)(buffer.size() + newLinePos - data);
               }
               case LENGTH:
                    if (expectedSize - buffer.size() <= (size_t)(end - data)) {
                         auto remaining = expectedSize - buffer.size();
                         buffer += std::string(data, data + remaining);
                         data = data + remaining;
                         bool rtn = OnBufferedData(&buffer[0], buffer.size());
                         buffer.clear();
                         if (rtn)
                              return true;
                    }
                    else {
                         goto cant_process;
                    }
               }
          }
     cant_process:
          buffer += std::string(data, end);
          return false;
     }

     ResponseBuilder::ResponseBuilder(Response* _response) : response(_response), state(MessageState::START_LINE) {}

     bool ResponseBuilder::OnBufferedData(const char* data, std::size_t length) {
          std::string buffer;
          const char* end = &data[length];
          switch (state.streamState) {
          case MessageState::START_LINE: {
               data = readWord(data, end, buffer);
               assert(buffer == "HTTP/1.1");

               data = readWord(data, end, buffer); // Status num 
               auto status = atoi(&buffer[0]);

               data = readWord(data, end, buffer); // Status text 

               response->status = new HttpStatus(status, buffer);
               expectLinefeed(data, end);
          }
               state = MessageState::HEADERS;
               return false;
          case  MessageState::HEADERS:

               if (data[0] == '\r' || data[0] == '\n') {
                    data += 2;
                    state.bodyType = BodyType::From(response->ContentLength(), response->TransferEncoding());
                    state.streamState = response->ContentLength() == 0 ? MessageState::FINISHED : MessageState::BODY;
                    bufferMode = response->TransferEncoding() == Encoding::chunked ? NEWLINE : NONE;
               }
               else {
                    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> conversion;
                    std::string buffer2;
                    data = readHeaderName(data, end, buffer);
                    data = readHeaderValue(data, end, buffer2);
                    auto header = rikitiki::Header(conversion.from_bytes(&buffer[0]), conversion.from_bytes(&buffer2[0]));
                    *response << header;
                    data += 2; // Burn \r\n                    
               }
               return state.streamState == MessageState::FINISHED;
          case  MessageState::BODY: {
               switch (state.bodyType) {
               case  BodyType::CHUNKED:
               {
                    response->Body().write(data, (std::streamsize)(expectedSize - 2));
                    if (expectedSize - 2 == 0)
                         state = MessageState::FINISHED;
                    else
                         state = MessageState::BODY;
                    bufferMode = NEWLINE;
                    break;
               }
               case BodyType::KNOWN_SIZE:{
                    if (data != end) {
                         if (response->TransferEncoding() == Encoding::chunked) {
                              expectedSize = (size_t)strtol(data, (char**)&data, 16) + 2;
                              data += 2; // burn \r\n
                              bufferMode = LENGTH;
                              state.bodyType = BodyType::CHUNKED;
                              return false;
                         }
                         else {
                              bufferMode = NONE;
                              response->Body().write(data, end - data);
                         }
                    }

                    if (response->ContentLength() != (uint64_t)-1 && response->Body().str().size() >= response->ContentLength())
                         state = MessageState::FINISHED;

                    break;
               } // case BodyType::BODY
               } // switch BodyType
               break;
          } // case  MessageState::BODY


          case  MessageState::FINISHED:
               throw new std::exception("Unexpected input");
          }

          return state.streamState == MessageState::FINISHED;
     }
     */


}