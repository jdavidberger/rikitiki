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
          stream << mxcomp::utf::convert(m.Startline()) << std::endl;
          for (auto header : m.Headers()){
               stream << mxcomp::utf::convert(header.first) << ": " << 
		 mxcomp::utf::convert(header.second) << std::endl;
          }
          stream << std::endl << std::endl;
          stream << m.Body().str();
          
          return stream;
     }
     std::wostream& operator<< (std::wostream& stream, const Message& m){
          stream << m.Startline() << std::endl;
          for (auto header : m.Headers()){
               stream << header.first << ": " << header.second << std::endl;
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
