#pragma once
#define RETURN(x) -> decltype(x) { return x; }

template <typename T> struct instance_of {
  T& value;
};
