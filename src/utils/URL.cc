#include <rikitiki/utils/URL.h>

namespace rikitiki {

#define _TOWSTRINGLITERAL(x) L ## x
#define TOWSTRINGLITERAL(x) _TOWSTRINGLITERAL(x)

     template<typename C> const C * ChooseCW(const char * c, const wchar_t * w);
     template<> const char * ChooseCW<char>(const char * c, const wchar_t*)
     {
          return c;
     }
     template<> const wchar_t *ChooseCW<wchar_t>(const char*, const wchar_t * w)
     {
          return w;
     }

#define _T(x, y) ChooseCW<x>(y, TOWSTRINGLITERAL(y) )

     template <class T>
     URL_<T>::URL_(const std::basic_string<T>& _uri) : uri(_uri) {
          start = &uri[0];
          end = &uri[uri.length()];
          hostStart = protocolStart = resourceStart = end;
          auto protocolEnd = uri.find( _T(T, "://") );
          if (protocolEnd != std::string::npos) {
               protocolStart = start;
               hostStart = &uri[protocolEnd + 3];
          }
          else {
               hostStart = start;
          }

          auto hostEnd = uri.find(_T(T, "/"), (unsigned)(hostStart - start));
          if (hostEnd != std::string::npos) {
               resourceStart = &uri[hostEnd];
          }

     }
}
