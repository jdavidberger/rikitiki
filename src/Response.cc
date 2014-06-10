#include <rikitiki/Response.h>
#include <assert.h>
#include <locale>
#include <codecvt>

namespace rikitiki {
     Response::Response() : ResponseType(ContentType::ToString(ContentType::DEFAULT)),
     status(&HttpStatus::OK){}

     Response& Response::operator <<(const rikitiki::HttpStatus& t){
          status = &t;
          return *this;
     }

     void Response::reset(){
          response.clear();
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

     Response& Response::operator <<(const rikitiki::Header& header){
          headers.push_back(header);
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

     ResponseBuilder::ResponseBuilder(Response* _response) : response(_response), expectedSize((std::size_t) - 1), state(STATUS) {}
     bool ResponseBuilder::OnData(const char* data, std::size_t length) {
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

          case HEADERS:
               do {
                    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> conversion;
                    std::string buffer2;
                    data = readHeaderName(data, end, buffer);
                    data = readHeaderValue(data, end, buffer2);
                    if (buffer == "Content-Length")
                         expectedSize = (std::size_t)atoi(&buffer2[0]);

                    response->headers.push_back(rikitiki::Header(conversion.from_bytes(&buffer[0]), 
                                                                 conversion.from_bytes(&buffer2[0])));
                    data += 2; // Burn \r\n
               } while (*data != '\n' && *data != '\r');
               expectLinefeed(data, end);

               state = PAYLOAD;
          case PAYLOAD:
               response->response.write(data, end - data);
               if (response->response.str().size() >= expectedSize)
                    state = FINISHED;
               break;
          case FINISHED:
               throw new std::exception("Unexpected input");
          }

          return state == FINISHED;
     }
}