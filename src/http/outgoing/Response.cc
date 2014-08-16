#include <rikitiki/http/outgoing/Response.h>
#include <assert.h>
#include <locale>

namespace rikitiki {
     size_t OResponseWriter::WriteStartLine() {
          return WriteData("HTTP/1.1 ") +
               WriteData(status->StartString() + "\r\n");
     }
     OResponseWriter& OResponseWriter::operator << (const rikitiki::HttpStatus& t) {
          status = &t;
          return *this;
     }

     OResponseMemory& OResponseMemory::operator << (const rikitiki::HttpStatus& t) {
          Response::operator<<(t);
          return *this;
     }
     OResponseMemory& OResponseMemory::operator <<(const rikitiki::Header &t){
          Response::operator<<(t);
          return *this;
     }
     OResponseMemory& OResponseMemory::operator <<(const rikitiki::Cookie &t){
          Response::operator<<(t);
          return *this;
     }
     OResponseMemory& OResponseMemory::operator <<(rikitiki::ContentType::t t){
          Response::operator<<(t);
          return *this;
     }

     size_t OResponseMemory::WritePayloadData(const char* b, size_t size) {
          OResponse::WritePayloadData(b, size);
          Body().write(b, (std::streamsize)size);
          return size;
     }
     void OResponseMemory::WriteHeader(const Header& h) {
          Response::operator<<(h);
     }

     OResponse& OResponse::operator << (rikitiki::ContentType::t t) {
          OMessage::operator<<(t);
          return *this;
     }

     OResponse& OResponse::operator <<(const rikitiki::Cookie& t)  {
          OMessage::operator<<(t);
          return *this;
     }
     OResponse& OResponse::operator <<(const rikitiki::Header& t)  {
          OMessage::operator<<(t);
          return *this;
     }
}
