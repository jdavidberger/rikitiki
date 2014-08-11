#include <mxcomp/log.h>
#include <rikitiki/http/Header.h>
#include <rikitiki/http/Message.h>
#include <locale>
#include <algorithm>
#include <codecvt>
#include <assert.h>

namespace rikitiki {
     Message& Message::operator <<(const rikitiki::Header& header){          
          Headers().Add(header.first, header.second);
          return *this;
     }

     Message& Message::operator <<(rikitiki::ContentType::t t){
          SetContentType(t);
          return *this;
     }

     Message& Message::operator <<(const rikitiki::Cookie& cookie){
          auto header = Header(L"Set-Cookie", cookie.first + L"=" + cookie.second);
          return *this << header;
     }

     size_t Message::ContentLength() const {          
          return Headers().Get<size_t>(L"Content-Length", (size_t)-1);
     }

     Encoding::t Message::TransferEncoding() const {
          return Headers().Get<Encoding::t>(L"Transfer-Encoding", Encoding::UNKNOWN);
     }

     ContentType::t Message::ContentType() const {
          return Headers().Get<ContentType::t>(L"contenttype", ContentType::text_html);
     }

     void Message::SetContentLength(size_t l) {
          Headers().Set(L"Content-Length", l);          
     }
     void Message::SetTransferEncoding(Encoding::t enc){
          Headers().Set(L"Transfer-Encoding", Encoding::ToString(enc));          
     }
     void Message::SetContentType(ContentType::t t){
          Headers().Set(L"Content-Type", ContentType::ToString(t));
     }

     std::ostream& operator<< (std::ostream& stream, const Message& m){
          std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
          stream << converter.to_bytes(m.Startline()) << std::endl;
          for (auto header : m.Headers()){
               stream << converter.to_bytes(header.first) << ": " << 
                    converter.to_bytes(header.second) << std::endl;
          }
          stream << std::endl << std::endl;
          stream << m.Body().str();
          
          return stream;
     }
     std::wostream& operator<< (std::wostream& stream, const Message& m){
          std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
          stream << m.Startline() << std::endl;
          for (auto header : m.Headers()){
               stream << header.first << ": " << header.second << std::endl;
          }
          stream << std::endl << std::endl;
          stream << converter.from_bytes( m.Body().str() );

          return stream;
     }

     const HeaderCollection& Message::Headers() const {
          HeaderCollection& rtn = const_cast<Message*>(this)->Headers();
          return rtn;
     }

     const ByteStream& Message::Body() const {
          ByteStream& rtn = const_cast<Message*>(this)->Body();
          return rtn;
     }

     const CookieCollection& Message::Cookies() const {
          CookieCollection& rtn = const_cast<Message*>(this)->Cookies();
          return rtn;
     }

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