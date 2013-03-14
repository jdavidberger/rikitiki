#pragma once
#define RETURN(x) -> decltype(x) { return x; }
#define STATIC_RETURN(x) -> decltype(x) { static const auto _instance = x; return _instance; }

template <typename T> struct instance_of {
  T& value;
};

namespace tupleExt {
template <typename F>
struct With {
  template <std::size_t N = 0, typename... Args, typename... FArgs>
    static inline typename std::enable_if<N == sizeof...(Args), void>::type
    iterate(const std::tuple<Args...>& t, FArgs&...){ }
    
    template <std::size_t N = 0, typename... Args, typename... FArgs>
    static inline typename std::enable_if<N < sizeof...(Args), void>::type
    iterate(const std::tuple<Args...>& t, FArgs&... fargs){
      F::evaluate( std::get<N>(t), fargs... );
      iterate<N+1, Args...>(t, fargs...);
    }
};
}
