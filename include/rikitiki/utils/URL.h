#include <string>

namespace rikitiki {
     template <typename class T>
     struct URL_ {
          std::basic_string<T> uri;
          const T *start, *end,
               *protocolStart,
               *hostStart,
               *resourceStart;
          URL_(const std::basic_string<T>& uri);
     };
     typedef URL_<char > URL;
     typedef URL_<wchar_t > wURL;
}