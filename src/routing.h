#pragma once
#include "server.h"
#include "connContext.h"

struct Route : public Handler { 
  std::string route;
  ConnContext::Method method;

  Route(const std::string& _route, ConnContext::Method _method);  
  Route(const std::string& _route);
  virtual std::string name() const;
};

template <typename... T>
static int modern_sscanf(const char* b, const char* format, T&... t);

template <typename P, typename F, typename... T> 
  struct Route_ : public Route {  
  P* parent;
  //  typedef void (P::*F)(ConnContext& ctx, FArgs...);
  F f;
  
 Route_(P* p, const std::string& _route, F _f, ConnContext::Method method) : parent(p), f(_f), Route(_route, method) { }
  
  int Scan(ConnContext& ctx, T&... t){
    return modern_sscanf(ctx.URI(), route.c_str(), t...);
  }
  virtual bool Handle(ConnContext& ctx){
    if( method != ConnContext::ANY && method != ctx.RequestMethod() ) {
      LOG(Web, Verbose) << "Not using route " << route << ", " << method << " for method failure." << std::endl;
      return false;
    }
    auto args = std::tuple_cat( std::tuple<ConnContext&>(ctx),
				std::tuple<T...>());
    
    int matched = applyTuple(this, &Route_<P, F, T...>::Scan, args);
    if(matched == sizeof...(T)){
      LOG(Web, Verbose) << "Using route " << route << ", " << method << std::endl;
      applyTuple(parent, f, args);
      return true;
    }
    return false;
  }
};

template <typename P, typename F> 
  struct Route_<P, F> : public Route {  
  P* parent;
  //  typedef void (P::*F)(ConnContext& ctx);
  F f;
    
 Route_(P* p, 	   
	const std::string& _route, 
	F _f,
	ConnContext::Method method) : parent(p), f(_f), Route(_route, method) {          
  }
    
  virtual bool Handle(ConnContext& ctx){
    bool shouldAttempt = 
      strcmp(route.c_str(), ctx.URI()) == 0 &&
      (method == ConnContext::ANY ||
       method == ctx.RequestMethod());

    if(shouldAttempt){
      LOG(Web, Verbose) << "Using route " << route << ", " << method << std::endl;
      (parent->*f)(ctx);
      return true;
    }
    return false;
  }
};


    /*    template <typename P, typename... Args>
    static Route_<P, F, T...>* make_route(P* p, 	   
						const std::string& _route, 
						void (P::*_f)(ConnContext& ctx, Args...),
						ConnContext::Method method = ConnContext::ANY){
      return new Route_<P, 
	void (P::*_f)(ConnContext& ctx, Args...), 
	T...>(p, _route, _f, method);
	}*/

template <typename T, bool Enable = std::is_fundamental<T>::value >
struct sane_ref_type{
  typedef const T& type;
};

template <typename T>
struct sane_ref_type<T, true>{
  typedef T type;
};

using namespace std;
template <typename... T> 
  struct CreateRoute {

    template<typename P>
    static Route* With(P* p, 	   
		       const std::string& _route, 
		       void (P::*_f)(ConnContext& ctx, typename sane_ref_type<T>::type...),
		       ConnContext::Method method = ConnContext::ANY){
      return new Route_<P, 
	void (P::*)(ConnContext& ctx, typename sane_ref_type<T>::type...), 
	T...>(p, _route, _f, method);
    }

    template<typename P>
    static Route* With(P* p, 	   
		       const std::string& _route, 
		       ConnContext::Method method  = ConnContext::ANY){            
      return new Route_<P, 
	void (P::*)(ConnContext& ctx, typename sane_ref_type<T>::type...), 
	T...>(p, _route, &P::operator(), method);
    }
      
  };


struct RouteHandler : public Handler {
  std::vector<Route*> routes;
  RouteHandler();
  
  virtual std::string name() const; 
  virtual bool Handle(ConnContext& ctx);

  void Add(Route* r){
    routes.push_back(r);
  }
  /*
  template <typename... T>
    void Add(const std::string& route, 
	     void (P::*F)(ConnContext& ctx, T...), 
	     ConnContext::Method method = ConnContext::ANY );

  template <typename... T>
    void Add(const std::string& route, 
	     ConnContext::Method method = ConnContext::ANY);
  */
};

#include "routing.tcc"
