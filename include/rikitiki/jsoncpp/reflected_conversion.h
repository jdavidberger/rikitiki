#include <rikitiki/reflection/reflection.h>
#include <tuple>

namespace rikitiki {
  template <typename T> struct json_value {
    template <typename S>
    static auto set(Json::Value& jv, T& t, const Field_<T, S>& field) -> decltype( jv[field.name] << field.get(t), void()) {
      if(!jv.isObject()) throw std::runtime_error("Expected an object in " + field.name);
      jv[field.name] << field.get(t);
    }   

    template <typename S>
    static auto set(Json::Value& jv, T& t, const Field_<T, S>& field) -> decltype(jv[field.name] = field.get(t), void()) {
      if(!jv.isObject()) throw std::runtime_error("Expected an object in " + field.name);
      jv[field.name] = field.get(t);
    }

    template <typename S> 
    static void get(Json::Value& jv, T& t, const Field_<T, S>& field){
      if(!jv.isObject()) throw std::runtime_error("Expected an object in " + field.name);
      jv[field.name] >> (&t)->*(field.field);
    }
    static void get(Json::Value& jv, T& t, const Field_<T, bool>& field){
      if(!jv.isObject()) throw std::runtime_error("Expected an object in " + field.name);
      field.set(t, jv[field.name].asBool());
    }
    static void get(Json::Value& jv, T& t, const Field_<T, double>& field){
      if(!jv.isObject()) throw std::runtime_error("Expected an object in " + field.name);
      field.set(t, jv[field.name].asDouble());
    }
    static void get(Json::Value& jv, T& t, const Field_<T, unsigned int>& field){
      if(!jv.isObject()) throw std::runtime_error("Expected an object in " + field.name);
      field.set(t, jv[field.name].asUInt());
    }
    static void get(Json::Value& jv, T& t, const Field_<T, int>& field){
      if(!jv.isObject()) throw std::runtime_error("Expected an object in " + field.name);
      field.set(t, jv[field.name].asInt());
    }
    static void get(Json::Value& jv, T& t, const Field_<T, std::string>& field){
      if(!jv.isObject()) throw std::runtime_error("Expected an object in " + field.name);
      field.set(t, jv[field.name].asString());
    }    
  };

  template <typename T>
    struct SetJsonValues {
      Json::Value& jv; T& t; 

    SetJsonValues(Json::Value& _jv, T& _t) : jv(_jv), t(_t) {}
      template <typename S>
      inline void operator()( const Field_<T, S>& field) {
	try {
	  json_value<T>::set(jv, t, field);
	} catch(std::exception& ex){
	  throw std::runtime_error("Error in setting field '" + std::string(field.name) + "': " + ex.what());
	}
      }
    };

  template <typename T>
    struct GetJsonValues {

      Json::Value& jv; T& t; 
    GetJsonValues(Json::Value& _jv, T& _t) : jv(_jv), t(_t) {}

      template <typename S>
      inline void operator()( const Field_<T, S>& field) {
	try {
	  json_value<T>::get(jv, t, field);
	} catch(std::exception& ex){
	  throw std::runtime_error("Error in getting field '" + std::string(field.name) + "': " + ex.what());
	}    
      }
    };

  template <typename T>
    static auto operator>>(Json::Value& jv, T& t) -> decltype(MetaClass_<T>::fields(), jv) {
    tupleExt::iterate(GetJsonValues<T>(jv, t), MetaClass_<T>::fields() );
    return jv;
  }

  template <typename T>
    static auto operator<<(Json::Value& jv, T& t) -> decltype(MetaClass_<T>::fields(), jv) {
    tupleExt::iterate(SetJsonValues<T>(jv, t), MetaClass_<T>::fields());
    return jv;
  };
}
