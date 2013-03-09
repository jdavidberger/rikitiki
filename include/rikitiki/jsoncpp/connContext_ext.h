/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include "../connContext.h"
#include <jsoncpp/json/json.h>

rikitiki::Response& operator <<(rikitiki::Response& response,  const Json::Value&); 

namespace rikitiki{
  rikitiki::ConnContext& operator>>(rikitiki::ConnContext& response, Json::Value& val);
}
