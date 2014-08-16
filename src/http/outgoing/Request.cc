#include <rikitiki/http/outgoing/Request.h>

namespace rikitiki {
     size_t ORequestMemory::WritePayloadData(const char* data, size_t size) {
          ORequest::WritePayloadData(data, size);
          Body().write(data, (std::streamsize)size);
          return size;
     }
     void ORequestMemory::WriteHeader(const Header& header) {
          ORequest::WriteHeader(header);
          Request::operator<<(header);
     }
}
