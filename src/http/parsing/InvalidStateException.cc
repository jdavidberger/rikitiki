#include <rikitiki/http/parsing/InvalidStateException.h>
namespace rikitiki {
     InvalidStateException::InvalidStateException(const std::string& msg) : rikitiki::exception(&msg[0]) {};
}
