#pragma once 
#include <string>

namespace web {
  namespace ContentType {
    enum t {
      application_pdf,
      application_javascript,
      
      text_html,
      text_css,
      text_xml,
      text_plain
    };
    
    std::string ToString(t type);
  }
}

