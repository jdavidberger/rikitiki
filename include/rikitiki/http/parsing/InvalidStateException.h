#pragma once
#include <string>
#include <rikitiki/exception.h>

namespace rikitiki {
     struct InvalidStateException : public rikitiki::exception {
          InvalidStateException(const std::string& msg);
     };
}
