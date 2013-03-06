/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

/* The original concept for the tuple functionality below was largely found at http://stackoverflow.com/a/1547118. I've expanded on it
   as need be - JB */
#pragma once
#include <tuple>

namespace tuple_ext {

template < uint N >
struct apply {
  template < typename T, typename... ArgsF, typename... ArgsT, typename... Args >
    static void applyTuple( T* pObj,
			    void (T::*f)( ArgsF... ),
			    std::tuple<ArgsT...>& t,
			    Args&... args ) {
    apply<N-1>::applyTuple( pObj, f, t, std::get<N-1>( t ), args... );
  }

  template < typename T, typename... ArgsF, typename... ArgsT, typename... Args >
    static void applyTuple_obj(T* pObj, 
			   void (T::*f)( ArgsF... ),
			   std::tuple<ArgsT...>& t,
			   Args&... args ) {
    apply<N-1>::applyTuple_obj( pObj, f, t, std::get<N-1>( t ), args... );
  }

  template < typename T, typename retF, typename... ArgsF, typename... ArgsT, typename... Args >
    static retF applyTuple_obj_ret(T* pObj, 
			   retF (T::*f)( ArgsF... ),
			   std::tuple<ArgsT...>& t,
			   Args&... args ) {
    return apply<N-1>::applyTuple_obj_ret( pObj, f, t, std::get<N-1>( t ), args... );
  }

};

template <>
struct apply<0> {
  template < typename T, typename... ArgsF, typename... ArgsT, typename... Args >
    static void applyTuple_obj( T* pObj,
			    void (T::*f)( ArgsF... ),
			    std::tuple<ArgsT...>& /* t */,
			    Args&... args )
  {
    (pObj->*f)( args... );
  }

  template < typename T, typename retF, typename... ArgsF, typename... ArgsT, typename... Args >
    static retF applyTuple_obj_ret( T* pObj,
			    retF (T::*f)( ArgsF... ),
			    std::tuple<ArgsT...>& /* t */,
			    Args&... args )
  {
    return (pObj->*f)( args... );
  }
};

template < typename... ArgsF, typename... ArgsT >
  void applyTuple( void (*f)(ArgsF...),
		   std::tuple<ArgsT...>& t )
{
  apply<sizeof...(ArgsT)>::applyTuple( f, t );
}

template < typename T, typename... ArgsF, typename... ArgsT >
  void applyTuple( T* pObj, void (T::*f)(ArgsF...), std::tuple<ArgsT...>& t )
{
  apply<sizeof...(ArgsT)>::applyTuple_obj(pObj, f, t );
}

template < typename T, typename retF, typename... ArgsF, typename... ArgsT >
  retF applyTuple( T* pObj, retF (T::*f)(ArgsF...),
		   std::tuple<ArgsT...>& t )
{
  return apply<sizeof...(ArgsT)>::applyTuple_obj_ret(pObj, f, t );
}

};
