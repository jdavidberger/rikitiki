/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include <rikitiki/jsoncpp/jsoncpp.h>
#include <rikitiki/utils/config.h>
using namespace rikitiki;

rikitiki::Response& operator <<(rikitiki::Response& response,  const Json::Value& json){   
  if(response.ResponseType == ContentType::DEFAULT)
    response << ContentType::application_json; 
  response.response << json;
  return response;
}
namespace rikitiki {
  rikitiki::ConnContext& operator>>(rikitiki::ConnContext& ctx, Json::Value& val){
    Json::Reader reader;
    bool parsingSuccessful = reader.parse( ctx.Payload(), val);
    if ( !parsingSuccessful )
      {
	ctx  << "Failed to parse configuration\n"
	     << reader.getFormatedErrorMessages();
	throw HandlerException(HttpStatus::Bad_Request);
      }  
    return ctx;
  }
}
