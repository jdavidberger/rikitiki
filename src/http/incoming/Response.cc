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

}