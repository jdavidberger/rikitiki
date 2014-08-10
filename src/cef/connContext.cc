/* Copyright (C) 2012-2013 Justin Berger
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include "connContext.h"

namespace rikitiki {
     namespace cef {

          void Request::FillHeaders(HeaderCollection& _headers) const {
               CefRequest::HeaderMap map;
               request->GetHeaderMap(map);
               for (auto header : map) {
                    _headers.insert(header);
               }
          }

          void Request::FillRequestMethod(Request::Method& m) const {
               auto method = request->GetMethod();
               m = RequestMethod::FromString(method.c_str());
          }

          const wchar_t* Request::URI() const{
               if (url.size() == 0) {
                    url = request->GetURL();
               }
               auto protocol = url.find(L"://") + 3;
               auto startFrom = url.find(L"/", protocol);
               return &url[startFrom];
          }

     }
}
