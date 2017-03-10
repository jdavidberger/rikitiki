#include <mxcomp/log.h>
#include <mxcomp/utf.h>

#include <rikitiki/http/Header.h>
#include <rikitiki/http/Message.h>

#include <locale>
#include <algorithm>
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
          auto header = Header(RT_STRING_LITERAL"Set-Cookie", cookie.first + RT_STRING_LITERAL"=" + cookie.second);
          return *this << header;
     }

     size_t Message::ContentLength() const {          
          return Headers().Get<size_t>(RT_STRING_LITERAL"Content-Length", (size_t)-1);
     }

     Encoding::t Message::TransferEncoding() const {
          return Headers().Get<Encoding::t>(RT_STRING_LITERAL"Transfer-Encoding", Encoding::UNKNOWN);
     }

     ContentType::t Message::ContentType() const {
          return Headers().Get<ContentType::t>(RT_STRING_LITERAL"contenttype", ContentType::text_html);
     }

     void Message::SetContentLength(size_t l) {
          Headers().Set(RT_STRING_LITERAL"Content-Length", l);
     }
     void Message::SetTransferEncoding(Encoding::t enc){
          Headers().Set(RT_STRING_LITERAL"Transfer-Encoding", Encoding::ToString(enc));
     }
     void Message::SetContentType(ContentType::t t){
          Headers().Set(RT_STRING_LITERAL"Content-Type", ContentType::ToString(t));
     }

     std::ostream& operator<< (std::ostream& stream, const Message& m){
          stream << rikitiki::to_string(m.Startline()) << std::endl;
          for (auto header : m.Headers()){
               stream << rikitiki::to_string(header.first) << ": " <<
                      rikitiki::to_string(header.second) << std::endl;
          }
          stream << std::endl << std::endl;
          stream << m.Body().str();
          
          return stream;
     }
     std::wostream& operator<< (std::wostream& stream, const Message& m){
          stream << rikitiki::to_wstring(m.Startline()) << std::endl;
          for (auto header : m.Headers()){
               stream << rikitiki::to_wstring(header.first) << ": " <<
                      rikitiki::to_wstring(header.second) << std::endl;
          }
          stream << std::endl << std::endl;
          stream << mxcomp::utf::convert( m.Body().str() );

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

}
