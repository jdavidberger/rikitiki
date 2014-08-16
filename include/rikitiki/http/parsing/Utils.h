#pragma once

#include <string>
#include <cstring>

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
}
