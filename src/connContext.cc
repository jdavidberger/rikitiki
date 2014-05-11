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
     Header::Header(const std::wstring& name, const std::wstring& value) : wstringpair(name, value){}

     PostContent::PostContent(const std::wstring& name, const std::wstring& value) : wstringpair(name, value){}

     Cookie::Cookie(const std::wstring& name, const std::wstring& value,
          const std::wstring& Domain, const std::wstring& Path,
          const std::wstring& Expires, bool secure, bool httpOnly) {
          first = name;
          second = value + L"; ";
          //    if(Domain.size())
          second += L"Domain=" + Domain + L"; ";
          if (Path.size())
               second += L"Path=" + Path + L"; ";
          if (Expires.size())
               second += L"Expires=" + Expires + L"; ";
          if (secure)
               second += L"Secure; ";
          if (httpOnly)
               second += L"HttpOnly";
     }

     ConnContext::Method RequestContext::RequestMethod() {
          if (_method == ANY){
               this->FillRequestMethod();
               assert(_method != ANY);
          }
          return _method;
     }

     static inline const wchar_t* read_to_name(const wchar_t* b){
          while (*b == ' ' || *b == ';') b++;
          return b;
     }
     static inline const wchar_t* read_name(const wchar_t* b){
          while (*b != '=' && *b != '\0') b++;
          return b;
     }

     static inline const wchar_t* read_value(const wchar_t* b){
          while (*b != ';' && *b != '\0') b++;
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
     void ConnContext::FillAccepts() {
          _accepts = new std::multimap<double, ContentType::t>();
          const wchar_t* b;
          b = &(Headers()[L"accept"])[0];
          while (*b != 0){
               const wchar_t* ee = read_accept_entry(b);
               double q = 1.0;
               const wchar_t* ct_begin, *ct_end;
               ct_begin = ct_end = 0;
               while ((b = read_accept_next_entryfield(b, ee)) != ee){
                    const wchar_t* efe = read_accept_entryfield(b, ee);
                    if (wcsncmp(b, L"q=", 2) == 0) {
                         b += 3;
                         b = read_double(q, b);
                    }
                    else {
                         ct_begin = b;
                         ct_end = efe;
                    }
                    b = efe;
               }
               _accepts->insert(std::make_pair(-q, ContentType::FromString(std::wstring(ct_begin, ct_end - ct_begin))));
               b = ee;
               while (*b == ',' || *b == ' ') b++;
          }
          if (_accepts->size() == 0)
               _accepts->insert(std::make_pair(-1, ContentType::ALL));
          mappedContentType = true;
     }

     void ConnContext::FillContentType() {
          _contentType = ContentType::FromString(Headers()[L"content-type"]);
          mappedContentType = true;
     }

     void ConnContext::FillPost(){
          mapContents(Payload(), _post);
          mappedPost = true;
     }
     
     std::multimap<double, ContentType::t>& ConnContext::Accepts(){
          if (_accepts == 0){
               FillAccepts();
               assert(_accepts);
          }
          return *_accepts;
     }

     ContentType::t ConnContext::ContentType() {
          if (!mappedContentType){
               this->FillContentType();
               assert(mappedContentType);
          }
          return _contentType;
     }
     std::string& ConnContext::Payload() {
          if (!mappedPayload){
               this->FillPayload();
               assert(mappedPayload);
          }
          return _payload;
     }

     HeaderCollection::value_type& RequestContext::AddRequestHeader(const wchar_t* _name, const wchar_t* value){
          std::wstring name(_name);
          std::transform(name.begin(), name.end(), name.begin(), ::tolower);
          auto& newHeader = *_headers.insert(std::make_pair(name,
               std::wstring(value)));

          return newHeader;
     }

     HeaderCollection& RequestContext::Headers(){
          if (!mappedHeaders){
               this->FillHeaders();
               assert(mappedHeaders);
          }
          return _headers;
     }

     CookieCollection& RequestContext::Cookies(){
          if (!mappedCookies){
               this->FillCookies();
               assert(mappedCookies);
          }
          return _cookies;
     }
     QueryStringCollection& RequestContext::QueryString() {
          if (!mappedQs){
               this->FillQueryString();
               assert(mappedQs);
          }
          return _qs;
     }

     void RequestContext::FillCookies(){
          auto range = Headers().equal_range(L"cookie");
          for (auto it = range.first; it != range.second; it++){
               LOG(Server, Debug) << "Req. cookie: " << it->first << " = " << it->second << std::endl;
               if (it->second.size() == 0) continue;
               const wchar_t* n = &it->second[0];
               const wchar_t *ne, *ve;
               do {
                    n = read_to_name(n);

                    ne = read_name(n);
                    ve = read_value(ne);
                    _cookies[std::wstring(n, ne)] =
                         ne >= ve ? std::wstring() : std::wstring(ne + 1, ve);
                    n = ve;
                    while (*n == ';' || *n == ' ') n++;
               } while (*n != 0);
          }
          mappedCookies = true;
     }


     PostCollection& ConnContext::Post() {
          if (!mappedPost){
               this->FillPost();
               assert(mappedPost);
          }
          return _post;
     }

     Response::Response() : ResponseType(rikitiki::ContentType::DEFAULT),
          status(&HttpStatus::OK){}

     Response& Response::operator <<(const rikitiki::HttpStatus& t){
          status = &t;
          return *this;
     }

     void Response::reset(){
          response.clear();
          headers.clear();
     }

     Response& Response::operator <<(rikitiki::ContentType::t t){
          ResponseType = t;
          return *this;
     }

     Response& Response::operator <<(const rikitiki::Cookie& cookie){
          return *this << Header(L"Set-Cookie", cookie.first + L"=" + cookie.second);
     }

     Response& Response::operator <<(const rikitiki::Header& header){
          headers.push_back(header);
          return *this;
     }

     ConnContext& ConnContext::operator<<(std::function<void(std::wostream&)> f){
          handled = true;
          f(response.response);
          return *this;
     }

     ConnContext& operator>>(ConnContext& ctx, std::string& t){
          t = ctx.Payload();
          return ctx;
     }
     RequestContext::~RequestContext(){ }
     RequestContext::RequestContext() : _method(ANY), mappedQs(false), mappedHeaders(false), mappedCookies(false) {}

     ConnContext::ConnContext(Server* _server) :
          handled(false),
          server(NULL),
          mappedPost(false),
          mappedPayload(false),
          mappedContentType(false),
          headersDone(false),
          _accepts(0) {
          server = _server;
     }
     ConnContext::ConnContext() : 
          handled(false),
          server(NULL),
          mappedPost(false),
          mappedPayload(false),
          mappedContentType(false),
          headersDone(false),
          _accepts(0) {}

     ConnContext::~ConnContext(){
          delete _accepts;
     }
     
     void ConnContextWithWrite::writeResponse() {

     }
     void ConnContextWithWrite::Close() {
          this->writeResponse();
     }
     ConnContextWithWrite::ConnContextWithWrite(Server* s) : ConnContext(s) {}
     void ConnContextWithWrite::operator delete(void* ptr) {
          ConnContextWithWrite* ctx = static_cast<ConnContextWithWrite*>(ptr);
          ctx->Close();
          ::operator delete(ptr);
     }

#define MATCH_METHOD_ENUM(eval)	do{if(wcscmp(method, L#eval) == 0) return ConnContext::eval;}while(false)

     ConnContext::Method strToMethod(const wchar_t* method){
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
          return ConnContext::ANY;
     }
     void mapQueryString(const wchar_t* _qs, std::map<std::string, std::string>& qs){
          if (_qs == NULL) return;
          std::string name;
          std::string* val = 0;
          bool nameMode = true;
          while (*_qs != '\0'){
               if (nameMode){
                    if (*_qs == '='){
                         nameMode = false;
                         val = &qs[name];
                    }
                    else if (*_qs == '&'){
                         qs[name] = "";
                         name.clear();
                    }
                    else {
                         name += *_qs;
                    }
               }
               else {
                    if (*_qs == '&'){
                         nameMode = true;
                         name.clear();
                    }
                    else {
                         *val += *_qs;
                    }
               }
               _qs++;
          }
     }

     ConnContext& ConnContext::operator <<(const Cookie& obj) {
          handled = true;
          response << obj;
          return *this;
     }

     ConnContext& ConnContext::operator <<(const Header& obj) {
          handled = true;
          response << obj;
          return *this;
     }

     void unescapeString(wchar_t* s, wchar_t* e, std::wstring& rtn) {
          rtn.reserve(e - s);
          for (s; s < e; s++) {
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
     void mapContents(std::wstring& raw_content, PostCollection& post){
          if (raw_content.size() == 0)
               return;
          if (raw_content.back() != L'&')
               raw_content.push_back(L'&');

          std::replace(raw_content.begin(), raw_content.end(), L'+', L' ');
          auto l_it = raw_content.begin();
          std::wstring name, value;
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
                    post.insert(PostContent(name, value.c_str()));
#else 
                    char* value = curl_unescape(&*l_it, it - l_it);
                    post.insert(PostContent(name, value));
                    curl_free(value);
#endif
                    l_it = it+1;
                    break;
               }
          }
     }
}
