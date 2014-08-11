/* Copyright (C) 2012-2013 Justin Berger
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include <mxcomp\log.h>
#include <rikitiki/connContext.h>
#include <algorithm>
#ifndef _MSC_VER
#include <curl/curl.h>
#endif
#include <assert.h>
#include <cstring>

namespace rikitiki {
     void type_conversion_error(ConnContext& ctx, void** handlers){
          std::wstring accepts;
          for (auto _type = 0; _type < (int)ContentType::MAX; _type++)
               if (handlers[_type]){
               if (accepts.size())
                    accepts += L", ";
               accepts += ContentType::ToString((ContentType::t)_type);
               }
          ctx << Header(L"Accept", accepts);
          throw rikitiki::HandlerException(HttpStatus::Not_Acceptable);
     }

     std::ostream& operator <<(std::ostream& response, const wchar_t* obj)
     {
          std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
          response << conv.to_bytes(obj);
          return response;
     }
     std::ostream& operator <<(std::ostream& response, const std::wstring& obj) {
          return response << obj.c_str();
     }

     std::wostream& operator <<(std::wostream& response, const std::string& obj) {
          return response << obj.c_str();
     }

     static inline const wchar_t* read_name(const wchar_t* b){
          while (*b != '=' && *b != '\0') b++;
          return b;
     }

 
     static inline const wchar_t* read_accept_entry(const wchar_t* b){
          while (*b != 0 && *b != ',') b++;
          return b;
     }
     static inline const wchar_t* read_accept_next_entryfield(const wchar_t* b, const wchar_t* be){
          while (b != be && (*b == ' ' || *b == ';'))b++;
          return b;
     }
     static inline const wchar_t* read_accept_entryfield(const wchar_t* b, const wchar_t* be){
          while (b != be && *b != ';') b++;
          return b;
     }
     static inline const wchar_t* read_double(double& v, const wchar_t* b) {
          v = 0;
          while (*b >= '0' && *b <= '9'){
               v *= 10;
               v += (double)(*b - '0');
               b++;
          }
          if (*b == '.'){
               b++;
               double den = 1.0;
               while (*b >= '0' && *b <= '9'){
                    den = den / 10.0;
                    v += (double)(*b - '0') * den;
                    b++;
               }
          }
          return b;
     }

     ConnContext::ConnContext(Server* _server, rikitiki::Request& _request, OResponse& _response) :
          handled(false),
          server(NULL), Request(_request), Response(_response) {
          server = _server;
     }

     ConnContext::~ConnContext(){
          
     }
     void ConnContext::Close() {      
          Response.Done();
          /*
          if (headersDone == false) {
               OnHeadersFinished();
               headersDone = true;
          } 
          */
     }
     
     ConnContext& ConnContext::operator <<(const HttpStatus& obj) {
          handled = true;
          Response << obj;
          return *this;
     }
     ConnContext& ConnContext::operator <<(const Cookie& obj) {
          handled = true;
          Response << obj;
          return *this;
     }
     
     ConnContext& ConnContext::operator <<(const Header& obj) {
          handled = true;
          Response << obj;
          return *this;
     }

     void unescapeString(wchar_t* s, wchar_t* e, std::wstring& rtn) {
          assert(e > s);
          rtn.reserve((std::wstring::size_type)(e - s));
          for (; s < e; s++) {
               if (*s == '%' && s + 2 < e) {
                    s++;
                    wchar_t z = s[2];
                    s[2] = 0; // most we can read is 2
                    rtn.push_back((wchar_t)wcstol(s, 0, 16));
                    s[2] = z;
                    s++;
               }
               else {
                    rtn.push_back(*s);
               }
          }
     }
     void mapContents(ByteStream& raw_content_stream, PostCollection& post){
          std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> conversion;

          std::wstring raw_content = conversion.from_bytes(raw_content_stream.str());

          if (raw_content.size() == 0)
               return;
          if (raw_content.back() != L'&')
               raw_content.push_back(L'&');

          std::replace(raw_content.begin(), raw_content.end(), L'+', L' ');
          auto l_it = raw_content.begin();
          std::wstring name;
          for (auto it = raw_content.begin(); it != raw_content.end(); it++){
               switch (*it){
               case L'=':
                    name = std::wstring(l_it, it);
                    l_it = it + 1;
                    break;
               case L'&':

#ifdef _MSC_VER
                    std::wstring value;
                    unescapeString(&*l_it, &*it, value);
                    post.insert(PostContent(name, value));
#else 
                    char* value = curl_unescape(&*l_it, it - l_it);
                    post.insert(PostContent(name, value));
                    curl_free(value);
#endif
                    l_it = it + 1;
                    break;
               }
          }
     }
}
