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
     std::ostream& operator <<(std::ostream& response, const wchar_t* obj)
     {
          std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
          response << conv.to_bytes(obj);
          return response;
     }
     std::ostream& operator <<(std::ostream& response, const std::wstring& obj) {
          return response << obj.c_str();
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
               _accepts->insert(std::make_pair(-q, ContentType::FromString(std::wstring(ct_begin, (std::wstring::size_type)(ct_end - ct_begin)))));
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

     ByteStream& RequestContext::Payload() {
          if (!mappedPayload){
               this->FillPayload();
               assert(mappedPayload);
          }
          return _payload;
     }

     HeaderCollection::value_type& RequestContext::AddRequestHeader(const std::wstring& _name, const std::wstring& value){
          std::wstring name(_name);
          std::transform(name.begin(), name.end(), name.begin(), ::towlower);
          auto& newHeader = *_headers.insert(std::make_pair(name, value));
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

     /*
     ConnContext& ConnContext::operator<<(std::function<void(std::wostream&)> f){
     handled = true;
     f(response.payload);
     return *this;
     }
     */
     ConnContext& operator>>(ConnContext& ctx, std::wstring& t){
          std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> conversion;
          t = conversion.from_bytes(ctx.Payload().str());
          return ctx;
     }
     RequestContext::~RequestContext(){ }
     RequestContext::RequestContext() :
          mappedPayload(false), _method(ANY), mappedQs(false), mappedHeaders(false), mappedCookies(false) {}

     ConnContext::ConnContext(Server* _server) :
          handled(false),
          server(NULL),
          mappedPost(false),
          mappedContentType(false),
          headersDone(false),
          _accepts(0) {
          server = _server;
     }
     ConnContext::ConnContext() :
          handled(false),
          server(NULL),
          mappedPost(false),
          mappedContentType(false),
          headersDone(false),
          _accepts(0) {}

     ConnContext::~ConnContext(){
          delete _accepts;
     }
     void ConnContextWithWrite::WriteHeaders() {
          std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
          std::stringstream ss;
          ss << "HTTP/1.1 " << response.status->status << " " << response.status->name << "\r\n";
          ss << "Content-Type: " << conv.to_bytes(response.ResponseType) << "\r\n";
          
          if(response.TransferEncoding == Encoding::chunked) {
               ss << "Transfer-Encoding: chunked" << "\r\n";                
          }
          else if (response.ContentLength != -1) {
               ss << "Content-Length: " << response.ContentLength << "\r\n";
          }
          
          for (auto it : response.headers){
               ss << conv.to_bytes(it.first) << ": " << conv.to_bytes(it.second) << "\r\n";
          }
          ss << "\r\n";
          std::string buffer = ss.str();

          rawWrite(buffer.c_str(), buffer.length());
     }
     void ConnContextWithWrite::OnHeadersFinished() {
          if (response.TransferEncoding == Encoding::chunked || response.ContentLength != -1) {
               WriteHeaders();
          }          
     }
     void ConnContextWithWrite::OnData() {
          if (response.TransferEncoding == Encoding::chunked) {
               std::stringstream ss;
               response.payload.swap(ss);
               std::string buffer = ss.str();
               auto len = buffer.size();
               if (len == 0)
                    return;

               std::stringstream resp;
               resp << std::hex << len << "\r\n" << buffer << "\r\n";
               buffer = resp.str();
               rawWrite(buffer.c_str(), buffer.length());
          }
     }
     
     ConnContextWithWrite::~ConnContextWithWrite() {
          
     }
     void ConnContext::Close() {
          if (headersDone == false) {
               OnHeadersFinished();
               headersDone = true;
          }          
     }
     void ConnContextWithWrite::Close() {          
          ConnContext::Close();
          if (response.TransferEncoding != Encoding::chunked) {
               auto body = response.payload.str();
               response.ContentLength = body.size();
               WriteHeaders();
               rawWrite(&body[0], body.size());
          } else {
               rawWrite("0\r\n\r\n", 5);
          }
     }
     ConnContextWithWrite::ConnContextWithWrite(Server* s) : ConnContext(s) {}


#define MATCH_METHOD_ENUM(eval)	{if(wcscmp(method, L#eval) == 0) return ConnContext::eval;}

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

#define MATCH_METHOD_STR(eval) case ConnContext::eval: return L#eval;
     const wchar_t* methodToStr(ConnContext::Method method){
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
          case ConnContext::ANY:
          case ConnContext::OTHER:
          default:
               LOG(Server, Error) << "methodToStr failed on str '" << method << "'" << std::endl;
               return L"ANY";
          }
     }

     void mapQueryString(const wchar_t* _qs, std::map<std::wstring, std::wstring>& qs){
          if (_qs == NULL) return;
          std::wstring name;
          std::wstring* val = 0;
          bool nameMode = true;
          while (*_qs != L'\0'){
               if (nameMode){
                    if (*_qs == L'='){
                         nameMode = false;
                         val = &qs[name];
                    }
                    else if (*_qs == L'&'){
                         qs[name] = L"";
                         name.clear();
                    }
                    else {
                         name += *_qs;
                    }
               }
               else {
                    if (*_qs == L'&'){
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
     ConnContext& ConnContext::operator <<(const HttpStatus& obj) {
          handled = true;
          response << obj;
          return *this;
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
                    l_it = it + 1;
                    break;
               }
          }
     }
}
