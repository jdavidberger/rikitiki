#include <rikitiki/http/Response.h>
#include <assert.h>
#include <locale>
#include <codecvt>

namespace rikitiki {
     Response::Response() : ResponseType(ContentType::ToString(ContentType::DEFAULT)),
     status(&HttpStatus::OK), TransferEncoding(Encoding::UNKNOWN), ContentLength((uint64_t)-1) {}

     Response& Response::operator <<(const rikitiki::HttpStatus& t){
          status = &t;
          return *this;
     }

     void Response::reset(){
          body.clear();
          headers.clear();
     }

     Response& Response::operator <<(rikitiki::ContentType::t t){
          ResponseType = ContentType::ToString(t);
          return *this;
     }

     Response& Response::operator <<(const rikitiki::Cookie& cookie){
          auto header = Header(L"Set-Cookie", cookie.first + L"=" + cookie.second);          
          return *this << header;
     }

     Response::~Response() {

     }
     Encoding::t Encoding::FromString(const wchar_t * str) {
          if (wcscmp(str, L"chunked") == 0)
               return Encoding::chunked; 
          if (wcscmp(str, L"compress") == 0)
               return Encoding::compress;
          if (wcscmp(str, L"deflate") == 0)
               return Encoding::deflate;
          if (wcscmp(str, L"gzip") == 0)
               return Encoding::gzip;
          if (wcscmp(str, L"identity") == 0)
               return Encoding::identity;
          return Encoding::OTHER;
     }
     Response& Response::operator <<(const rikitiki::Header& header){
          if (header.first.compare(L"Content-Length") == 0) {
               ContentLength = (uint64_t)_wtoi(&header.second[0]);
          }
          else if (header.first.compare(L"Transfer-Encoding") == 0) {
               TransferEncoding = Encoding::FromString(&header.second[0]);
          }
          else if (header.first.compare(L"Content-Type") == 0) {
               ResponseType = header.second;
          }
          IMessage::operator<<(header);
          return *this;
     }

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

     ResponseBuilder::ResponseBuilder(Response* _response) : response(_response), 
          expectedSize((std::size_t) - 1), state(STATUS), bufferMode(NEWLINE) {}
     bool ResponseBuilder::OnData(const char* data, std::size_t length) {
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
     
     bool ResponseBuilder::OnBufferedData(const char* data, std::size_t length) {
          std::string buffer;
          const char* end = &data[length];
          switch (state) {
          case STATUS: {               
               data = readWord(data, end, buffer);
               assert(buffer == "HTTP/1.1");

               data = readWord(data, end, buffer); // Status num 
               auto status = atoi(&buffer[0]);
               
               data = readWord(data, end, buffer); // Status text 
               
               response->status = new HttpStatus(status, buffer);              
               expectLinefeed(data, end);
          }
          state = HEADERS;
          return false;
          case HEADERS:
               
               if (data[0] == '\r' || data[0] == '\n') {
                    data += 2; 
                    state = response->ContentLength == 0 ? FINISHED : PAYLOAD;
                    bufferMode = response->TransferEncoding == Encoding::chunked ? NEWLINE : NONE;
               } else {
                    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> conversion;
                    std::string buffer2;
                    data = readHeaderName(data, end, buffer);
                    data = readHeaderValue(data, end, buffer2);
                    auto header = rikitiki::Header(conversion.from_bytes(&buffer[0]), conversion.from_bytes(&buffer2[0]));
                    *response << header;
                    data += 2; // Burn \r\n                    
              }
               return state == FINISHED;
          case CHUNK_PAYLOAD: {
               response->Body().write(data, (std::streamsize)(expectedSize  - 2));
               if (expectedSize - 2 == 0)
                    state = FINISHED;
               else
                    state = PAYLOAD;
               bufferMode = NEWLINE;
               break;
          }
          case PAYLOAD:
               if (data != end) {
                    if (response->TransferEncoding == Encoding::chunked) {       
                              expectedSize = (size_t)strtol(data, (char**)&data, 16) + 2;
                              data += 2; // burn \r\n
                              bufferMode = LENGTH;
                              state = CHUNK_PAYLOAD;
                              return false;
                    }
                    else {
                         bufferMode = NONE;
                         response->Body().write(data, end - data);
                    }
               }

               if (response->ContentLength != (uint64_t)-1 && response->Body().str().size() >= response->ContentLength)
                    state = FINISHED;
                
               break;
          case FINISHED:
               throw new std::exception("Unexpected input");
          }

          return state == FINISHED;
     }
}