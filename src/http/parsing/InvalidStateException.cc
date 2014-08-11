#pragma once
#include <rikitiki\http\parsing\InvalidStateException.h>
namespace rikitiki {
     InvalidStateException::InvalidStateException(const std::string& msg) : std::exception(&msg[0]) {};
}