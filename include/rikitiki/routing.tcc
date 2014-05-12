// -*-c++-*-
/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#pragma once

// ----------- Parse format string -----------------
inline bool skip_to_format(std::wstringstream& b, const wchar_t*& format){
  while( *format != '{' ){
    if(b.peek() != *format) return false;    
    if(b.peek() == '\0') return false;
    b.get();
    format++;
  }     
  return true;
}

inline bool skip_to_unformat(const wchar_t*& format){
  while( *format != '}' ){
    if(*format == '\0') return false;
    format++;
  } 
  format++;
  return true;
}

template <typename T>
inline static bool extract(std::wstringstream& b, T& t){
  b >> t;  
  return !b.fail();
}

template <>
inline bool extract(std::wstringstream& b, std::wstring& t){
  while(b.good()){
    int v = b.get();
    bool accept = 
      (v >= '0' && v <= ';') ||
      (v >= '@' && v <= 'Z') ||
      (v >= '_' && v <= 'z') ||
      (v == '%') ||
      (v == '-');
    if(accept)
      t += v;
    else {
      b.unget();
      break;
    }
  }
  return t.size() > 0;
}


static inline int modern_sscanf(std::wstringstream& b, const wchar_t*& format){
  while( *format != '\0' && !b.eof() ){
    if(b.peek() != *format) return false;    
    b.get(); if(!b.good()) return 0;
    format++;
  }       
  return *format == '\0' && (wchar_t)EOF == b.peek() ? 1 : 0;
} 

template <typename H, typename... T>
static int modern_sscanf(std::wstringstream& b, const wchar_t*& format, H& h, T&... t){
  if(!skip_to_format(b, format)) return 0;
  skip_to_unformat(format);
  return extract(b, h) ? 
    1 + modern_sscanf(b, format, t...) :
    0;
}

template <typename... T>
static int modern_sscanf(const wchar_t* b, const wchar_t* format, T&... t){
  std::wstringstream ss(b);
  return modern_sscanf(ss, format, t...);
}

//------------------ Route_ -----------------------
template <typename P, typename... T> 
Route_<P,T...>::Route_(P* p, const std::wstring& _route, F _f, ConnContext::Method method) : parent(p), f(_f), Route(_route, method) { }

template <typename P, typename... T>
int Route_<P, T...>::Scan(ConnContextRef ctx, T&... t){
     return modern_sscanf(ctx->URI(), route.c_str(), t...);
}

template <typename P, typename... T>
int Route_<P, T...>::ScanTest(RequestContext& ctx, T&... t){
     return modern_sscanf(ctx.URI(), route.c_str(), t...);
}

template <typename P, typename... T>
bool Route_<P, T...>::CanHandle(RequestContext& ctx){
     if (method != ConnContext::ANY && method != ctx.RequestMethod()) {
          return false;
     }

     auto args = std::tuple_cat(std::tuple<RequestContext&>(ctx), std::tuple<T...>());

     int matched = mxcomp::tuples::applyTuple(this, &Route_<P, T...>::ScanTest, args);
     if (matched == sizeof...(T) + 1){
          return true;
     }
     return false;
}

template <typename P, typename... T> 
bool Route_<P,T...>::Handle(ConnContextRef ctx){
  if( method != ConnContext::ANY && method != ctx->RequestMethod() ) {
    return false;
  }
  
  auto args = std::tuple_cat(std::tuple<ConnContextRef>(ctx), std::tuple<T...>());

  // If anyone can tell me why this applyTuple expands out completely,
  // but the one below [applyTuple(parent, f, args)] doesn't, I'd be 
  // super interested to hear it. 
  int matched = mxcomp::tuples::applyTuple(this, &Route_<P, T...>::Scan, args);
  if(matched == sizeof...(T) + 1){
    LOG(Web, Verbose) << "Using route " << route << ", " << method << std::endl;
    mxcomp::tuples::applyTuple(parent, f, args);
    return true;
  }
  return false;
}

//------------------ Route_ (no argument specialization) 
template <typename P> 
Route_<P>::Route_(P* p, 	   
		  const std::wstring& _route, 
		  F _f,
		  ConnContext::Method method) : parent(p), f(_f), Route(_route, method) {          
}

template <typename P>
bool Route_<P>::CanHandle(RequestContext& ctx){
     bool shouldAttempt =
          wcscmp(route.c_str(), ctx.URI()) == 0 &&
          (method == ConnContext::ANY ||
          method == ctx.RequestMethod());
     return shouldAttempt;
}

template <typename P> 
bool Route_<P>::Handle(ConnContextRef ctx){
  bool shouldAttempt = 
    wcscmp(route.c_str(), ctx->URI()) == 0 &&
    (method == ConnContext::ANY ||
     method == ctx->RequestMethod());
  
  if(shouldAttempt){
    LOG(Web, Verbose) << "Using route " << route << ", " << method << std::endl;
    (parent->*f)(ctx);
    return true;
  }
  return false;
}

//------------------ CreateRoute

template <typename... T> 
template<typename P>
Route* CreateRoute<T...>::With(P* p, 	   
			       const std::wstring& _route, 
			       typename Route_<P, T...>::F _f,
			       ConnContext::Method method){
  return new Route_<P, T...>(p, _route, _f, method);
	
}

template <typename... T> 
template<typename P>
Route* CreateRoute<T...>::With(P* p, 	   
			       const std::wstring& _route, 
			       ConnContext::Method method){
	return CreateRoute<T...>::With(p, _route, &P::operator(), method);
}
