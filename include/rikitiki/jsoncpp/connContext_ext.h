/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include "../connContext.h"
#include <jsoncpp/json/json.h>
#include <array>
namespace rikitiki{
  rikitiki::Response& operator <<(rikitiki::Response& response,  const Json::Value&); 
  rikitiki::ConnContext& operator>>(rikitiki::ConnContext& response, Json::Value& val);

  template <>
    struct ContentHandler_<Json::Value>  { 
    static constexpr std::array<ContentType::t,2> ContentTypes() { 
      return { {ContentType::application_json, ContentType::application_javascript } }; };
  };
  

  template <typename T>
    static Json::Value& operator >>(Json::Value& jv, std::vector<T>& t){
    assert(jv.isArray());
    t.resize(jv.size());
    for(unsigned int i = 0;i < jv.size();i++)
      jv[i] >> t[i];
    return jv;
  }

  template <typename T>
    static Json::Value& operator<<(Json::Value& jv, std::vector<T>& t){
    jv = Json::Value(Json::arrayValue);
    jv.resize(t.size());
    for(unsigned int i = 0;i < t.size();i++)
      jv[i] << t[i];
    return jv;
  }
}


