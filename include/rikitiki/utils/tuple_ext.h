/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */
#pragma once
#include <tuple>

namespace tupleExt {
  /** 
      The original concept for the tuple functionality below was largely found at http://stackoverflow.com/a/1547118. 
      
      It recursively unrolls a tuple, and applies it to function. 
      So basically if you have:
      
      tuple<Arg1, Arg2, Arg3, Arg4> tuple;

      applyTuple(t, &T::f, tuple)
      is equivalent to
      t->f( get<0>(tuple), get<1>(tuple), get<2>(tuple), get<3>(tuple));

      There are a few variations below, namely for return types and static functions. This should all probably 
      be part of the standard. 

      The modifications from the original posting were to change all the types to reference types, and add some overloads. 
  */
template < uint N >
struct apply {
  template < typename T, typename... ArgsF, typename... ArgsT, typename... Args >
    static inline void applyTuple( T* pObj,
			    void (T::*f)( ArgsF... ),
			    std::tuple<ArgsT...>& t,
			    Args&... args ) {
    apply<N-1>::applyTuple( pObj, f, t, std::get<N-1>( t ), args... );
  }

  template < typename T, typename... ArgsF, typename... ArgsT, typename... Args >
    static inline void applyTuple_obj(T* pObj, 
			   void (T::*f)( ArgsF... ),
			   std::tuple<ArgsT...>& t,
			   Args&... args ) {
    apply<N-1>::applyTuple_obj( pObj, f, t, std::get<N-1>( t ), args... );
  }

  template < typename T, typename retF, typename... ArgsF, typename... ArgsT, typename... Args >
    static inline retF applyTuple_obj_ret(T* pObj, 
			   retF (T::*f)( ArgsF... ),
			   std::tuple<ArgsT...>& t,
			   Args&... args ) {
    return apply<N-1>::applyTuple_obj_ret( pObj, f, t, std::get<N-1>( t ), args... );
  }

};

/**
   This is the specialization of apply which marks the termination of the recursive expansion.
   \sa apply
 */
template <>
struct apply<0> {
  template < typename T, typename... ArgsF, typename... ArgsT, typename... Args >
    static inline void applyTuple_obj( T* pObj,
			    void (T::*f)( ArgsF... ),
			    std::tuple<ArgsT...>& /* t */,
			    Args&... args )
  {
    (pObj->*f)( args... );
  }

  template < typename T, typename retF, typename... ArgsF, typename... ArgsT, typename... Args >
    static inline retF applyTuple_obj_ret( T* pObj,
			    retF (T::*f)( ArgsF... ),
			    std::tuple<ArgsT...>& /* t */,
			    Args&... args )
  {
    return (pObj->*f)( args... );
  }
};

template < typename... ArgsF, typename... ArgsT >
  static inline void applyTuple( void (*f)(ArgsF...),
		   std::tuple<ArgsT...>& t )
{
  apply<sizeof...(ArgsT)>::applyTuple( f, t );
}

 template < typename T, typename... ArgsF, typename... ArgsT >
   static inline void applyTuple( T* pObj, void (T::*f)(ArgsF...), std::tuple<ArgsT...>& t )
 {
   apply<sizeof...(ArgsT)>::applyTuple_obj(pObj, f, t );
 }

 template < typename T, typename retF, typename... ArgsF, typename... ArgsT >
   static inline retF applyTuple( T* pObj, retF (T::*f)(ArgsF...),
				  std::tuple<ArgsT...>& t )
 {
   return apply<sizeof...(ArgsT)>::applyTuple_obj_ret(pObj, f, t );
 }

 /** Tuple operations against a static functor wih a function named 'operate'. */
 template <typename F>
   struct With {
     /** iterate through, rely on side effects of evaluate to get things done */
     template <std::size_t N = 0, typename... Args, typename... FArgs>
       static inline typename std::enable_if<N < sizeof...(Args), void>::type
       iterate(const std::tuple<Args...>& t, FArgs&... fargs){
       F::evaluate( std::get<N>(t), fargs... );
       iterate<N+1, Args...>(t, fargs...);
     }
	
       template <std::size_t N = 0, typename... Args, typename... FArgs>
       static inline typename std::enable_if<N == sizeof...(Args), void>::type
       iterate(const std::tuple<Args...>& t, FArgs&...){ }
   };
  
 template <std::size_t N = 0, typename F, typename... Args>
   inline auto iterate(const F& , const std::tuple<Args...>&) -> typename std::enable_if<N == sizeof...(Args), void>::type { }

 template <std::size_t N = 0, typename F, typename... Args>
   inline auto iterate(F& f, const std::tuple<Args...>& t) -> typename std::enable_if<N < sizeof...(Args), void>::type{
   f( std::get<N>(t) );
   iterate<N+1, F, Args...>(f, t);
 }

 /** Exists so you can make the functor in place. */
 template <std::size_t N = 0, typename F, typename... Args>
   inline auto iterate(const F& _f, const std::tuple<Args...>& t) -> typename std::enable_if<N == 0, void>::type {
   F f(_f);
   iterate<N, F, Args...>(f, t);
 }
}

