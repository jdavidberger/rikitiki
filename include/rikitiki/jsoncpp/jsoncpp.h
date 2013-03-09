/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include "connContext_ext.h"

/** \page jsoncpp JsonCPP
 
The [JsonCPP](http://jsoncpp.sourceforge.net) extension lets you seamlessly write and read from connection contexts with overloads to the appropriate stream operators. 

For example:
~~~~~~~~
void operator()(ConnContext& ctx){
    JsonCpp::Value v;
    ctx >> v;
    ctx << v;
}
~~~~~~~~

First reads a json object from the request payload, and then writes out that json object back to the response stream. If the payload is not a valid json object, the stream operator will throw a HandlerException with a return status of 'Bad Request'; with the parsing error information. 

\sa examples/rest.h
*/
