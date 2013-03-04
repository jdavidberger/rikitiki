/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include "content_types.h"

#define CASE_RET(X,Y) case X ## _ ## Y: return #X "/" #Y;

namespace rikitiki {
  namespace ContentType {
    std::string ToString(t type){
      switch(type){
	CASE_RET(application, pdf);
	CASE_RET(application, javascript);
	CASE_RET(text, html);
	CASE_RET(text, css);
	CASE_RET(text, xml);
	CASE_RET(text, plain);
      }
      return "unknown";
    }
  }
}
