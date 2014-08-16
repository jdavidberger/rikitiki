/* Copyright (C) 2012-2013 Justin Berger
The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */
#include <rikitiki/http/Enums.h>
#include <rikitiki/http/http_statuses.h>

#include <mxcomp/useful_macros.h>
#include <mxcomp/cont_stringbuf.h>
#include <mxcomp/log.h>

#include <string>
#include <assert.h>

namespace rikitiki {
     namespace RequestMethod {

#define MATCH_METHOD_ENUM(eval)	{if(wcscmp(method, L###eval) == 0) return eval;}

          t FromString(const wchar_t* method){
               MATCH_METHOD_ENUM(GET);
               MATCH_METHOD_ENUM(POST);
               MATCH_METHOD_ENUM(HEAD);
               MATCH_METHOD_ENUM(PUT);
               MATCH_METHOD_ENUM(DELETE);
               MATCH_METHOD_ENUM(TRACE);
               MATCH_METHOD_ENUM(OPTIONS);
               MATCH_METHOD_ENUM(CONNECT);
               MATCH_METHOD_ENUM(PATCH);
               LOG(Server, Error) << "strToMethod failed on method '" << method << "'" << std::endl;
               return ANY;
          }

#define MATCH_METHOD_STR(eval) case eval: return L###eval;
          const wchar_t* ToString(t method){
               switch (method) {
                    MATCH_METHOD_STR(GET);
                    MATCH_METHOD_STR(POST);
                    MATCH_METHOD_STR(HEAD);
                    MATCH_METHOD_STR(PUT);
                    MATCH_METHOD_STR(DELETE);
                    MATCH_METHOD_STR(TRACE);
                    MATCH_METHOD_STR(OPTIONS);
                    MATCH_METHOD_STR(CONNECT);
                    MATCH_METHOD_STR(PATCH);
                    MATCH_METHOD_STR(ANY);
               case OTHER:
               default:
                    LOG(Server, Error) << "methodToStr failed on str '" << method << "'" << std::endl;
                    return L"ANY";
               }
          }

     }

     namespace Encoding {          
          const wchar_t* ToString(t) {
               assert(false);
               return L"";
          }
          t FromString(const wchar_t * str) {
               if (wcscmp(str, L"chunked") == 0)
                    return Encoding::chunked;
               if (wcscmp(str, L"compress") == 0)
                    return Encoding::compress;
               if (wcscmp(str, L"deflate") == 0)
                    return Encoding::deflate;
               if (wcscmp(str, L"gzip") == 0)
                    return Encoding::gzip;
               if (wcscmp(str, L"identity") == 0)
                    return Encoding::identity;
               return Encoding::OTHER;
          }

          std::wistream& operator >>(std::wistream& s, t& _t) {
               std::wstring str;
               s >> str;
               _t = FromString(str.data());
               return s;
          }          


     }

     std::string HttpStatus::StartString() const {
          std::stringstream ss;
          std::string rtn;
          auto buff = mxcomp::attachContinousBuffer(ss, rtn);
          ss << status << " " << name; 
          return rtn;
     }
}
