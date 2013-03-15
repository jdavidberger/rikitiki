#pragma once
#define RETURN(x) -> decltype(x) { return x; }
#define STATIC_RETURN(x) -> decltype(x) { static const auto _instance = x; return _instance; }

template <typename T> struct instance_of {
  T& value;
};

namespace tupleExt {
  template <typename F>
    struct With {
      /** Versions which work against a static functor */
      template <std::size_t N = 0, typename... Args, typename... FArgs>
	static inline typename std::enable_if<N < sizeof...(Args), void>::type
	iterate(const std::tuple<Args...>& t, FArgs&... fargs){
	F::evaluate( std::get<N>(t), fargs... );
	iterate<N+1, Args...>(t, fargs...);
      }

	template <std::size_t N = 0, typename... Args, typename... FArgs>
	static inline typename std::enable_if<N == sizeof...(Args), void>::type
	iterate(const std::tuple<Args...>& t, FArgs&...){ }

	/** Non-static functor */
	/*	  template <std::size_t N = 0, typename... Args>
	static inline typename std::enable_if<N < sizeof...(Args), void>::type
	iterate(F& functor, const std::tuple<Args...>& t){
	    functor(std::get<N>(t));
	    iterate<N+1, Args...>(functor, t);
	  }
	    
	    template <std::size_t N = 0, typename... Args>
	static inline typename std::enable_if<N == sizeof...(Args), void>::type
	iterate(F&, const std::tuple<Args...>& t){ }
	*/
    };
}
