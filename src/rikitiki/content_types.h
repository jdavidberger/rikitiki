/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#pragma once 
#include <string>

namespace rikitiki {
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

