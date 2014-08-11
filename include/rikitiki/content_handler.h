#pragma once

#include <rikitiki/http/content_types.h>
#include <vector>
#include <map>
#include <array>
#include <mxcomp/reflection.h>
#ifdef _MSC_VER
#define constexpr
#endif 

namespace rikitiki {
     class ConnContext;
     struct Response;

     template <typename T>
     struct ContentHandler_ {
          static constexpr std::array<ContentType::t, 1> ContentTypes() { return{ { ContentType::DEFAULT } }; };
     };

     struct OutProvider {
          template<typename S, typename T>
          static auto Make() -> void(*)(ConnContext&, S&) {
               return [](ConnContext& ctx, S& s) {
                    T t;
                    t << s;
                    ctx << t;
               };
          }
     };

     struct InProvider {
          template<typename S, typename T>
          static auto Make() -> void(*)(ConnContext&, S&) {
               return [](ConnContext& ctx, S& s) {
                    T t;
                    ctx >> t;
                    t >> s;
               };
          }
     };

     template <typename S, typename FProvider, typename... T>
     struct TypeConversions  {
          typedef TypeConversions<S, FProvider, T...> thisType;
          typedef TypeConversions<std::vector<S>, FProvider, T...> VectorType;
          template <typename Th, typename...Tt>
          struct HandlerAdders {
               static void Add(thisType* _this){
                    HandlerAdders<Th>::Add(_this);
                    HandlerAdders<Tt...>::Add(_this);
               }
          };

          template <typename Th>
          struct HandlerAdders<Th> {
               static auto Add(thisType* _this) -> void {
                    for (auto contentType : ContentHandler_<Th>::ContentTypes()){
                         assert(contentType > ContentType::DEFAULT &&
                              contentType < ContentType::MAX &&
                              "Invalid content type value in specialized handler");
                         _this->handlers[contentType] = FProvider::template Make<S, Th>();
                    }
               }
          };

          typedef void(*Setter)(ConnContext&, S& s);
          std::vector<Setter> handlers;

          TypeConversions() {
               handlers.resize(ContentType::MAX);
               HandlerAdders<T...>::Add(this);
          }

          static thisType& Instance() {
               static thisType Instance;
               return Instance;
          }
     };

     template<typename T, typename enable = void >
     struct valid_conversions { };

}

#ifdef USE_JSONCPP
#include <rikitiki/jsoncpp/jsoncpp>
namespace rikitiki {
     using namespace mxcomp;
     template<typename T>
     struct valid_conversions<T, typename std::enable_if< std::is_function <decltype(MetaClass_<T>::fields)>::value >::type > {
          typedef TypeConversions<T, InProvider, Json::Value> In;
          typedef TypeConversions<T, OutProvider, Json::Value> Out;      
     };

     template <typename T>
     struct valid_conversions<std::vector<T>, typename std::enable_if< std::is_class<valid_conversions<T> >::value >::type > {
          typedef typename valid_conversions<T>::In::VectorType In;
          typedef typename valid_conversions<T>::Out::VectorType Out;
     };


}

#endif