#include <rikitiki\Request.h>

namespace rikitiki {
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