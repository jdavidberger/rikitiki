#pragma once

#include <string>
#include <exception>

namespace rikitiki {
  class exception : public std::exception {
    std::string _what;
  public:
  exception(const std::string& w = "Rikitiki Error") : _what(w) {}
    virtual const char* what() const noexcept {
      return _what.data();
    }
  };
}
