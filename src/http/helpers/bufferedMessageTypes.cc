#include <rikitiki/http/helpers/bufferedMessageTypes.h>

namespace rikitiki {
     namespace http {
          namespace helpers {
               void BufferedMessage::FillContentLength(size_t& t) const {
                    t = Message::ContentLength();
               }
               void BufferedMessage::FillTransferEncoding(Encoding::t& t) const {
                    t = Message::TransferEncoding();
               }
               void BufferedMessage::FillContentType(ContentType::t& t) const {
                    t = Message::ContentType();
               }
               void BufferedMessage::FillCookies(CookieCollection& cookies) const {
                    auto _cookies = Message::Headers().GetList(L"Set-Cookie");
                    for (auto _cookie : _cookies) {
                         Cookie cookie(_cookie);
                         cookies.insert(std::make_pair(cookie.first, cookie.second));
                    }
               }
          }
     }
}
