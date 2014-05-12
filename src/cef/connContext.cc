/* Copyright (C) 2012-2013 Justin Berger
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include <rikitiki/cef/cef>
#include <cstring>
#include <mxcomp\log.h>

namespace rikitiki {
     namespace cef {

          const wchar_t* RequestContext::URI(){
               if (url.size() == 0) {
                    url = request->GetURL();
               }
               auto protocol = url.find(L"://") + 3;
               auto startFrom = url.find(L"/", protocol);
               return &url[startFrom];
          }

     }
}
