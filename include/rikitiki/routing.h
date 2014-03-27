/* Copyright (C) 2012-2013 Justin Berger
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#pragma once
#include "server.h"
#include "connContext.h"
#include <cstring>
namespace rikitiki {

     /**
        Specialization of handler which has a route (in the form of "/path/{variable}/{other-variable}")
        and also has a specification as to what http method it accepts.
        */
     struct Route : public Handler {
          std::string route;
          ConnContext::Method method;

          Route(const std::string& _route, ConnContext::Method _method);
          Route(const std::string& _route);
          virtual std::string name() const;
     };

     /**
        Maps compound types to 'const T&', others to 'T'.
        */
     template <typename T, bool Enable = std::is_compound<T>::value >
     struct sane_ref_type{
          typedef T type;
     };

     /**
        Maps compound types to 'const T&', others to 'T'.
        */
     template <typename T>
     struct sane_ref_type<T, true>{
          typedef const T& type;
     };


     template <typename C, typename rtn, typename... Args> struct Function_{
          typedef rtn(C::*Type)(typename sane_ref_type<Args>::type...);
     };

#ifdef _MSC_VER && 0

#define SRT(A) typename sane_ref_type<A>::type

     template <typename C, typename rtn, typename A1> struct Function_<C, rtn, A1> {
          typedef rtn(C::*type)(SRT(A1));
     };

     template <typename C, typename rtn, typename A1, typename A2> struct Function_<C, rtn, A1, A2> {
          typedef rtn(C::*type)(SRT(A1), SRT(A2));
     };

     template <typename C, typename rtn, typename A1, typename A2, typename A3> struct Function_<C, rtn, A1, A2, A3> {
          typedef rtn(C::*type)(SRT(A1), SRT(A2), SRT(A3));
     };

     template <typename C, typename rtn, typename A1, typename A2, typename A3, typename A4> struct Function_<C, rtn, A1, A2, A3, A4> {
          typedef rtn(C::*type)(SRT(A1), SRT(A2), SRT(A3), SRT(A4));
     };

     template <typename C, typename rtn, typename A1, typename A2, typename A3, typename A4, typename A5>
     struct Function_<C, rtn, A1, A2, A3, A4, A5> {
          typedef rtn(C::*type)(SRT(A1), SRT(A2), SRT(A3), SRT(A4), SRT(A5));
     };

     template <typename C, typename rtn, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
     struct Function_<C, rtn, A1, A2, A3, A4, A5, A6> {
          typedef rtn(C::*type)(SRT(A1), SRT(A2), SRT(A3), SRT(A4), SRT(A5), SRT(A6));
     };

     template <typename C, typename rtn, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
     struct Function_<C, rtn, A1, A2, A3, A4, A5, A6, A7> {
          typedef rtn(C::*type)(SRT(A1), SRT(A2), SRT(A3), SRT(A4), SRT(A5), SRT(A6), SRT(A7));
     };
#undef SRT(A)
#endif 

     /**
        A route incorporates the variadic logic needed to map the Handle(ConnContext&) call to
        the function type.
        \sa Route, Handler
        \param P Usually the module type that the route is for.
        \param T... The argument types for the function to call
        */

     template <typename P, typename... T>
     struct Route_ : public Route {
          typedef typename Function_<P, void, ConnContext&, T...>::type F;
          //typedef void (P::*F)(ConnContext& ctx, typename sane_ref_type<T>::type...);
          P* parent;
          F f;
          virtual bool visible() const { return false; }
          Route_(P* p, const std::string& _route, F _f, ConnContext::Method method);

          int Scan(ConnContext& ctx, T&... t);
          virtual bool Handle(ConnContext& ctx);
     };

     /** Specialization of Route_ for functions with no parameters.
         \sa Route_
         */
     template <typename P>
     struct Route_<P> : public Route{
          typedef void (P::*F)(ConnContext& ctx);
          P* parent;
          F f;
          virtual bool visible() const { return method == ConnContext::GET || method == ConnContext::ANY; }
          Route_(P* p, const std::string& _route, F _f, ConnContext::Method method);
          virtual bool Handle(ConnContext& ctx);
     };

     /**
        Convienence class, meant to alleviate having to type in the class that the
        route function works on in a similiar fasion to std::make_pair does for pairs.
        \sa Route_, Route
        */
     template <typename... T>
     struct CreateRoute {
          template<typename P>
          static Route* With(P* p,
               const std::string& _route,
               typename Route_<P, T...>::F _f,
               ConnContext::Method method = ConnContext::ANY);

          template<typename P>
          static Route* With(P* p,
               const std::string& _route,
               ConnContext::Method method = ConnContext::ANY);

     };

#include "routing.tcc"

}
