#pragma once
#include <string>
#include <exception>

namespace rikitiki {
     struct InvalidStateException : public std::exception {
          InvalidStateException(const std::string& msg);
     };
}