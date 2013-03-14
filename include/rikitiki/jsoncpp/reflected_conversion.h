#include <rikitiki/reflection/reflection.h>
#include <tuple>

namespace rikitiki {
  template <typename T> struct json_value {
    template <typename S>
    static auto set(Json::Value& jv, T& t, Field_<T, S>& field) -> decltype( jv[field.name] << field.get(t), void()) {
      if(!jv.isObject()) throw std::runtime_error("Expected an object in " + field.name);
      jv[field.name] << field.get(t);
    }
    
    template <typename S>
    static auto set(Json::Value& jv, T& t, Field_<T, S>& field) -> decltype(jv[field.name] = field.get(t), void()) {
      if(!jv.isObject()) throw std::runtime_error("Expected an object in " + field.name);
      jv[field.name] = field.get(t);
    }

    template <typename S> 
    static void get(Json::Value& jv, T& t, Field_<T, S>& field){
      if(!jv.isObject()) throw std::runtime_error("Expected an object in " + field.name);
      jv[field.name] >> (&t)->*(field.field);
    }
    static void get(Json::Value& jv, T& t, Field_<T, bool>& field){
      if(!jv.isObject()) throw std::runtime_error("Expected an object in " + field.name);
      field.set(t, jv[field.name].asBool());
    }
    static void get(Json::Value& jv, T& t, Field_<T, double>& field){
      if(!jv.isObject()) throw std::runtime_error("Expected an object in " + field.name);
      field.set(t, jv[field.name].asDouble());
    }
    static void get(Json::Value& jv, T& t, Field_<T, unsigned int>& field){
      if(!jv.isObject()) throw std::runtime_error("Expected an object in " + field.name);
      field.set(t, jv[field.name].asUInt());
    }
    static void get(Json::Value& jv, T& t, Field_<T, int>& field){
      if(!jv.isObject()) throw std::runtime_error("Expected an object in " + field.name);
      field.set(t, jv[field.name].asInt());
    }
    static void get(Json::Value& jv, T& t, Field_<T, std::string>& field){
      if(!jv.isObject()) throw std::runtime_error("Expected an object in " + field.name);
      field.set(t, jv[field.name].asString());
    }
    
  };


  template <int N>
    struct json_tuple {
      template <typename T, typename... Args>
	static inline void set_values(Json::Value& jv, T& t, std::tuple<Field_<T, Args>...> args){
	auto field = std::get<N>(args);
	try {
	  json_value<T>::set(jv, t, field);
	} catch(std::exception& ex){
	  throw std::runtime_error("Error in setting field '" + field.name + "': " + ex.what());
	}
	json_tuple<N-1>::set_values(jv, t, args);
      }
      template <typename T, typename... Args>
	static inline void get_values(Json::Value& jv, T& t, std::tuple<Field_<T, Args>...> args){
	auto field = std::get<N>(args);
	try {
	  json_value<T>::get(jv, t, field);
	} catch(std::exception& ex){
	  throw std::runtime_error("Error in getting field '" + field.name + "': " + ex.what());
	}
	json_tuple<N-1>::get_values(jv, t, args);
      }

    };	
	
  template <>
    struct json_tuple<-1> {
      template <typename T, typename... Args> 
	static inline void set_values(Json::Value& jv, T& t, std::tuple<Field_<T, Args>...> args){ }
      template <typename T, typename... Args> 
	static inline void get_values(Json::Value& jv, T& t, std::tuple<Field_<T, Args>...> args){ }
  };

  template <typename T>
    static auto operator>>(Json::Value& jv, T& t) -> decltype(MetaClass_<T>::fields() ,jv){
    auto fields = MetaClass_<T>::fields();
    json_tuple<std::tuple_size<decltype(fields)>::value - 1>::get_values(jv, t, fields);
    return jv;
  };

  template <typename T>
    static auto operator<<(Json::Value& jv, T& t) -> decltype(MetaClass_<T>::fields() ,jv){
    auto fields = MetaClass_<T>::fields();
    json_tuple<std::tuple_size<decltype(fields)>::value - 1>::set_values(jv, t, fields);
    return jv;
  };
}
