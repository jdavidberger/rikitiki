#include <rikitiki/http/Request.h>

namespace rikitiki {
     Header::Header(const rikitiki::string& name, const rikitiki::string& value) : wstringpair(name, value){}

     PostContent::PostContent(const rikitiki::string& name, const rikitiki::string& value) : wstringpair(name, value){}
     rikitiki::string Request::Startline() const {
          rikitiki::stringstream wss;
          wss << RequestMethod::ToString(RequestMethod()) << L" " << URI() << L" HTTP/1.1";
          return wss.str();
     }

     void Request::SetStartline(const rikitiki::string& startLine) {
          auto firstSpace = startLine.find(' ');
          auto lastSpace = startLine.rfind(' ');
          rikitiki::string method(startLine.data(), &startLine[firstSpace]);
          rikitiki::string uri(&startLine[firstSpace], &startLine[lastSpace]);
          SetRequestMethod(RequestMethod::FromString(&method[0]));
     }
}
