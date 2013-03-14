#include "../metaprogramming.h"
#include <type_traits>

namespace rikitiki {
  template <typename T> struct MetaClass_{    };

  template <typename T> 
    struct Member_ {
    Member_(const std::string& _name) : name(_name){}
      std::string name;
    };

  template <typename T, typename Rtn, typename... Args>
    struct Function_ : Member_<T> {
    using FunctionT = Rtn (T::*)(Args...);
    FunctionT function;
  Function_(const std::string& _name, FunctionT _function ) : function(_function), Member_<T>(_name) {}
    
    Rtn invoke(T& t, Args... args){
      return (t.*function)(args...);
    }
  };

  template <typename T, typename... Args>
    struct Function_<T, void, Args...> : Member_<T> {
    using FunctionT = void (T::*)(Args...);
    FunctionT function;
  Function_(const std::string& _name, FunctionT _function ) : function(_function), Member_<T>(_name) {}
    
    void invoke(T& t, Args... args){
      (t.*function)(args...);
    }
  };


  template <typename T, typename S>
    struct Field_ : Member_<T> {
    using FieldT = S (T::*);
    FieldT field;
  Field_(const std::string& _name, FieldT _field ) : field(_field), Member_<T>(_name) {}
      S& get(T& t) const {
	return (&t)->*field;
      }
      void set(T& t, const S& s) const{
	(&t)->*field = s;
      }
    };

  template <typename T, typename S, class Enable = void>
    struct SuperClass_ {
      SuperClass_(){ static_assert(std::is_base_of<S,T>::value, "Given parameters are invalid: S is not a subclass of T."); }
    };

  template <typename T, typename S>
    struct SuperClass_<T, S, typename std::enable_if< std::is_base_of<S, T>::value>::type >  { };

  template <typename T, typename Rtn, typename... Args>
    static auto make_function( const std::string& name, Rtn (T::*function)(Args...) )
    RETURN((Function_<T, Rtn, Args...>(name, function)))

  template <typename T, typename S>
    static auto make_field( const std::string& name, S (T::*field))
    RETURN((Field_<T, S>(name, field)))

  template <typename T, typename... Args> 
    static auto make_fields(Field_<T, Args>... args) 
    RETURN( std::make_tuple(args...) )   
}