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
          void Request::FillQueryString(QueryStringCollection& qs) const {
               auto _url = request->GetURL().ToWString();
               auto qmark = _url.find_last_of(L'?');  
               if (qmark != -1)
                    qs.FromQueryString(&_url[qmark + 1]);
          }
          const wchar_t* Request::URI() const{
               if (url.size() == 0) {
                    url = request->GetURL();                    
                    auto protocol = url.find(L"://") + 3;
                    auto startFrom = url.find(L"/", protocol);
                    auto qmark = url.find_last_of(L'?');
                    url = qmark == -1 ? std::wstring(&url[startFrom]) :
                                        std::wstring(&url[startFrom], &url[qmark]);
               }
               
               return url.data();
          }

     }
}
