#pragma once

#include <map>
#include <array>
namespace rikitiki {
  struct ConnContext; 
  struct Response;

  template <typename T>
    struct ContentHandler_ { 
      static constexpr std::array<ContentType::t,1> ContentTypes() { return { { ContentType::DEFAULT } }; };
  };

  struct OutProvider {
    template<typename S, typename T>
    static auto Make() -> void (*)(ConnContext&, S&) {
      return [](ConnContext& ctx, S& s) { 
	T t;
	t << s;
	ctx << t;
      };	
    }
  };
  
  struct InProvider {
    
    template<typename S, typename T>
    static auto Make() -> void (*)(ConnContext&, S&) {
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
	  static void Add( thisType* _this){
	    HandlerAdders<Th>::Add(_this);
	    HandlerAdders<Tt...>::Add(_this);
	  }
	};

      template <typename Th>
      struct HandlerAdders<Th> {
	static auto Add( thisType* _this) -> void {
	  for(auto contentType : ContentHandler_<Th>::ContentTypes()){
	    assert(contentType > ContentType::DEFAULT &&
		   contentType < ContentType::MAX && 
		   "Invalid content type value in specialized handler");	    
	    _this->handlers[contentType] = FProvider::template Make<S, Th> ();
	  }
	}
      };
      
      typedef void (*Setter)(ConnContext&, S& s);
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

  template<typename S>
    struct valid_conversions {
      
    };

  template <typename T>
    struct valid_conversions<std::vector<T>> {
    typedef typename valid_conversions<T>::In::VectorType In;
    typedef typename valid_conversions<T>::Out::VectorType Out;
  };

}
