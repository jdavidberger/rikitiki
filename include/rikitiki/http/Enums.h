/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#pragma once 
#include <string>

#ifdef DELETE
#undef DELETE
#endif

namespace rikitiki {
     namespace RequestMethod {
          enum t {
               ANY = 0, GET = 1, POST = 2, HEAD = 3, PUT = 4, DELETE = 5, TRACE = 6, OPTIONS = 7, CONNECT = 8, PATCH = 9, OTHER
          };

         t FromString(const char*);
         t FromString(const wchar_t*);
          const wchar_t* ToString(t);
          std::wistream& operator >>(std::wistream& s, t& _t);
     }

     namespace Encoding {
          enum t {
               UNKNOWN,
               chunked,
               compress,
               deflate,
               gzip,
               identity,
               OTHER
          };

          t FromString(const wchar_t*);
          const wchar_t* ToString(t);
          std::wistream& operator >>(std::wistream& s, t& _t);

     }

}

