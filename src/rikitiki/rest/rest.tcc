// -*-c++-*-
/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */
#include <cxxabi.h>

#define MakeCreateIfHandles(METHOD)					\
  template<typename T, typename... Args>				\
  static auto CreateIfHandles ## METHOD (T* t, const std::string& route, int) \
  -> decltype(CreateRoute<Args...>::With(t, route, &T::METHOD,		\
					 ConnContext::METHOD ))		\
  {									\
    return CreateRoute<Args...>::With(t, route, &T::METHOD,		\
				      ConnContext::METHOD );		\
  }									\
  template<typename T, typename... Args>				\
  static rikitiki::Route* CreateIfHandles ## METHOD (T* t, const std::string& route, long) { \
      return 0;								\
  }									

  
namespace rikitiki {
  namespace rest {
    MakeCreateIfHandles(GET);
    MakeCreateIfHandles(POST);
    MakeCreateIfHandles(PUT);
    MakeCreateIfHandles(DELETE);
    
    static int addIfNotNull(Server& server, Route* r){
      if(r != 0){
	server.AddHandler(r);
	return 1;
      }
      return 0;
    }
    
    template <typename T>
    int Register(Server& server, const std::string& route, const std::string& routeWId, T* t){
      int rtn = 
	addIfNotNull(server, CreateIfHandlesGET<T>(t, route,0)) + 
	addIfNotNull(server, CreateIfHandlesPOST<T>(t, route,0)) +
	addIfNotNull(server, CreateIfHandlesPUT<T>(t, route,0))+
	addIfNotNull(server, CreateIfHandlesDELETE<T>(t, route,0))+      
	addIfNotNull(server, CreateIfHandlesGET<T,int>(t, routeWId,0))+
	addIfNotNull(server, CreateIfHandlesPOST<T,int>(t, routeWId,0))+
	addIfNotNull(server, CreateIfHandlesPUT<T,int>(t, routeWId,0))+
	addIfNotNull(server, CreateIfHandlesDELETE<T,int>(t, routeWId,0));

      if(rtn == 0) {
	LOG(Rest, Error) << typeid(T) << " has no rest functions defined." << std::endl;
      } else {
	LOG(Rest, Info) << typeid(T) << " has " << rtn << " functions defined." << std::endl;
      }
      return rtn;
    }

    template <typename T>
    int Register(Server& server, const std::string& route, T* t){
      return Register(server, route, route + "/{id}", t);
    }
  }
}
