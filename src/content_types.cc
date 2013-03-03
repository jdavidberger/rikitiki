#include "content_types.h"

#define CASE_RET(X,Y) case X ## _ ## Y: return #X "/" #Y;

namespace web {
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
