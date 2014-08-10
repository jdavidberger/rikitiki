#include <rikitiki\http\Request.h>

namespace rikitiki {
     Header::Header(const std::wstring& name, const std::wstring& value) : wstringpair(name, value){}

     PostContent::PostContent(const std::wstring& name, const std::wstring& value) : wstringpair(name, value){}
     std::wstring Request::Startline() const {
          std::wstringstream wss;
          wss << RequestMethod::ToString(RequestMethod()) << L" " << URI() << L" HTTP/1.1";
          return wss.str();
     }

     void Request::SetStartline(const std::wstring& startLine) {
          auto firstSpace = startLine.find(' ');
          auto lastSpace = startLine.rfind(' ');
          std::wstring method(startLine.data(), &startLine[firstSpace]);
          std::wstring uri(&startLine[firstSpace], &startLine[lastSpace]);
          SetRequestMethod(RequestMethod::FromString(uri.data()));
     }
}