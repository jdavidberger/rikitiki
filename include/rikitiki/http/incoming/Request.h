#pragma once

#include <rikitiki/http/Request.h>
#include <rikitiki/http/incoming/Message.h>

namespace rikitiki {     

     struct IRequest : public virtual IMessage {
     
     };

     template <class T> struct IRequest_ : public IMessage_< T >, public IRequest {
          virtual RequestMethod::t RequestMethod();
          virtual const wchar_t* URI();

          virtual CookieCollection& Cookies();
          virtual QueryStringCollection& QueryString();
     };
     
}
