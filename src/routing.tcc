#pragma once
// -*-c++-*-

//------------------ Route_ -----------------------

inline bool skip_to_format(std::stringstream& b, const char*& format){
  while( *format != '{' ){
    if(b.peek() != *format) return false;    
    if(b.peek() == '\0') return false;
    b.get();
    format++;
  } 
  return true;
}

inline bool skip_to_unformat(const char*& format){
  while( *format != '}' ){
    if(*format == '\0') return false;
    format++;
  } 
  format++;
  return true;
}

static inline int modern_sscanf(std::stringstream& b, const char*& format){
  return 0;
} 

template <typename T>
inline static bool extract(std::stringstream& b, T& t){
  b >> t;
  return !b.fail();
}

template <>
inline bool extract(std::stringstream& b, std::string& t){
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
    else 
      return t.size() > 0;     
  }
  return t.size() > 0;
}


template <typename H, typename... T>
static int modern_sscanf(std::stringstream& b, const char*& format, H& h, T&... t){
  if(!skip_to_format(b, format)) return 0;
  skip_to_unformat(format);
  return extract(b, h) ? 
    1 + modern_sscanf(b, format, t...) :
    0;
}

template <typename... T>
static int modern_sscanf(const char* b, const char* format, T&... t){
  std::stringstream ss(b);
  return modern_sscanf(ss, format, t...);
}

//------------------ Route specialization -----------------------

//------------------ RouteHandler -----------------------

/*
template <typename P>
template <typename... T>
void RouteHandler<P>::Add(const std::string& route, void (P::*F)(ConnContext& ctx, T...), ConnContext::Method method){
  LOG(Web, Info) << "Adding handler for " << route << std::endl;
  Route* newRoute = Route_Utils<P, T...>::make_route(p, route, F, method);
  routes.push_back(newRoute);
}

template <typename P>
template <typename... T>
void RouteHandler<P>::Add(const std::string& route, ConnContext::Method method){
  Add<T...>(route, &P::operator(), method);
}
*/
