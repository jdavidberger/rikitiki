#pragma once

#include <rikitiki/http/Response.h>
#include <rikitiki/http/incoming/Message.h>

namespace rikitiki {

     struct IResponse : public virtual IMessage {

     };

     struct IResponseMemory : public IResponse, public virtual Response {

     };

}