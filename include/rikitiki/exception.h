#pragma once
#include <mxcomp/useful_macros.h>

#include <string>
#include <exception>

namespace rikitiki {
  class exception : public std::exception {
    std::string _what;
  public:
  exception(const std::string& w = "Rikitiki Error") : _what(w) {}
    virtual const char* what() const OVERRIDE {
      return _what.data();
    }
  };
}
