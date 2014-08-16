#pragma once

#include <string>
#include <map>
#include <sstream>
#include <mxcomp/useful_macros.h>
#include "Message.h"
#include "Enums.h"
#include <rikitiki/http/Header.h>

namespace rikitiki {     

     // http://www.w3.org/Protocols/rfc2616/rfc2616-sec5.html
     class Request : public virtual Message {
     public:          
          typedef RequestMethod::t Method;
          virtual RequestMethod::t RequestMethod() const = 0;
          virtual void SetRequestMethod(RequestMethod::t) = 0;

          virtual const wchar_t* URI() const = 0;

          virtual QueryStringCollection& QueryString() = 0;          

          virtual void SetStartline(const std::wstring&) OVERRIDE;
          virtual std::wstring Startline() const OVERRIDE;
     };
       
}
