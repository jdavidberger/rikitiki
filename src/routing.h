/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#pragma once
#include "server.h"
#include "connContext.h"

namespace rikitiki {

struct Route : public Handler { 
  std::string route;
  ConnContext::Method method;

  Route(const std::string& _route, ConnContext::Method _method);  
  Route(const std::string& _route);
  virtual std::string name() const;
};

template <typename T, bool Enable = std::is_fundamental<T>::value >
struct sane_ref_type{
  typedef const T& type;
};

template <typename T>
struct sane_ref_type<T, true>{
  typedef T type;
};

template <typename P, typename... T> 
  struct Route_ : public Route {  
  typedef void (P::*F)(ConnContext& ctx, typename sane_ref_type<T>::type...);
  P* parent;
  F f;
  
  Route_(P* p, const std::string& _route, F _f, ConnContext::Method method);
  
  int Scan(ConnContext& ctx, T&... t);
  virtual bool Handle(ConnContext& ctx);
};

template <typename P> 
  struct Route_<P> : public Route {  
  typedef void (P::*F)(ConnContext& ctx);
  P* parent;
  F f;    
  Route_(P* p, const std::string& _route, F _f, ConnContext::Method method);
  virtual bool Handle(ConnContext& ctx);
};

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
		       ConnContext::Method method  = ConnContext::ANY);
      
  };

#include "routing.tcc"

}
