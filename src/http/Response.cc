#include <rikitiki/http/Response.h>
#include <assert.h>
#include <locale>
#include <cstring>

namespace rikitiki {
     Response::Response() : status(&HttpStatus::OK) {}

     Response& Response::operator <<(const rikitiki::HttpStatus& t){
          status = &t;
          return *this;
     }

     Response::~Response() {
          if (!status->managed) {
               delete status;
               status = 0;
          }
     }
     void Response::SetStartline(const std::wstring& startline) {
          auto pos = startline.find(L' ');
          auto str = &startline[pos + 1];
          wchar_t* pos2 = 0;
          auto statusCode = (uint16_t)wcstol(str, &pos2, 10);
          std::wstring statusString(pos2+1);
          while ( ::iswspace( statusString[statusString.length()-1] ) )
               statusString.pop_back();
          
          status = new HttpStatus(statusCode, statusString);
     }
     std::wstring Response::Startline() const {
          std::wstringstream wss;
          wss << L"HTTP/1.1 " << status->status << L" " << status->name;
          return wss.str();
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
          out.resize((std::size_t)(br - data)); 
	  std::memcpy(&out[0], data, out.size());

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
     
}
