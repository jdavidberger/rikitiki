#include <rikitiki\Request.h>

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
     #define _T(T, STR) ChooseCW<T>(STR, TOWSTRINGLITERAL(STR))

     template <typename class T>
     URL_<T>::URL_(const std::basic_string<T>& _uri) : uri(_uri) {          
          start = &uri[0];
          end = &uri[uri.length()];
          hostStart = protocolStart = resourceStart = end;
          auto protocolEnd = uri.find(_T(T, "://"));
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

     template wURL;
     template URL;

     Header::Header(const std::wstring& name, const std::wstring& value) : wstringpair(name, value){}

     PostContent::PostContent(const std::wstring& name, const std::wstring& value) : wstringpair(name, value){}

     Cookie::Cookie(const std::wstring& name, const std::wstring& value,
          const std::wstring& Domain, const std::wstring& Path,
          const std::wstring& Expires, bool secure, bool httpOnly) {
          first = name;
          second = value + L"; ";
          //    if(Domain.size())
          second += L"Domain=" + Domain + L"; ";
          if (Path.size())
               second += L"Path=" + Path + L"; ";
          if (Expires.size())
               second += L"Expires=" + Expires + L"; ";
          if (secure)
               second += L"Secure; ";
          if (httpOnly)
               second += L"HttpOnly";
     }

}