#include "../metaprogramming.h"

namespace rikitiki {

  template <typename T> 
    struct MetaClass_{
      MetaClass_(){
	
      };
    };

  

  template <typename T, typename S>
    struct Field_ {
      typedef S (T::*FieldT);
      std::string name;
      FieldT field;
      S& get(T& t){
	return (&t)->*field;
      }
      void set(T& t, const S& s){
	(&t)->*field = s;
      }
    };

  template <typename T, typename S>
    static Field_<T, S> make_field( const std::string& name, S (T::*field)) {
    return Field_<T, S>{name, field};
  }

  template <typename T, typename... Args> 
    static std::tuple<Field_<T, Args>... > make_fields(Field_<T, Args>... args){
    return std::tuple<Field_<T, Args>...>(args...);
  }
}
