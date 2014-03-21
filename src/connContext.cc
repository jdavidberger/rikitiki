/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include <rikitiki/connContext.h>
#include <algorithm>
#ifndef _MSC_VER
#include <curl/curl.h>
#endif
#include <assert.h>
#include <cstring>

namespace rikitiki { 
  Header::Header(const std::string& name, const std::string& value) : stringpair(name, value){}

  PostContent::PostContent(const std::string& name, const std::string& value) : stringpair(name, value){}

  Cookie::Cookie(const std::string& name, const std::string& value, 
		 const std::string& Domain, const std::string& Path,
		 const std::string& Expires, bool secure, bool httpOnly) {
    first = name;
    second = value + "; ";
//    if(Domain.size())
	second += "Domain=" + Domain + "; ";
    if(Path.size())
      second += "Path=" + Path + "; ";
    if(Expires.size())
      second += "Expires=" + Expires + "; ";
    if(secure)
      second += "Secure; ";
    if(httpOnly)
      second += "HttpOnly";
  }

  ConnContext::Method ConnContext::RequestMethod() {
    if(_method == ANY){
      this->FillRequestMethod();
      assert(_method != ANY);
    }
    return _method;
  }

  static inline const char* read_to_name(const char* b){
    while(*b == ' ' || *b == ';') b++;
    return b;
  }
  static inline const char* read_name(const char* b){
    while(*b != '=' && *b != '\0') b++;
    return b;
  }

  static inline const char* read_value(const char* b){
    while(*b != ';' && *b != '\0') b++;
    return b;
  }

  static inline const char* read_accept_entry(const char* b){
    while(*b != 0 && *b != ',') b++;
    return b;
  }
  static inline const char* read_accept_next_entryfield(const char* b, const char* be){
    while(b != be && (*b == ' ' || *b == ';'))b++;
    return b;
  }
  static inline const char* read_accept_entryfield(const char* b, const char* be){
    while(b != be && *b != ';') b++;
    return b;
  }
  static inline const char* read_double(double& v, const char* b) {
    v = 0;
    while(*b >= '0' && *b <= '9'){
      v *= 10; 
      v += (double)(*b - '0');
      b++;
    } 
    if(*b == '.'){
      b++;
      double den = 1.0;
      while(*b >= '0' && *b <= '9'){
	den = den / 10.0;
	v += (double)(*b - '0') * den;
	b++;
      }       
    }
    return b;
  }
  void ConnContext::FillAccepts() {
    _accepts = new std::multimap<double, ContentType::t>();
    const char* b;
    b = &(Headers()["accept"])[0];    
    while(*b != 0){
      const char* ee = read_accept_entry(b);
      double q = 1.0;
      const char* ct_begin, *ct_end;
      ct_begin = ct_end = 0;
      while( (b = read_accept_next_entryfield(b, ee)) != ee){
	const char* efe = read_accept_entryfield(b, ee);
	if(strncmp(b, "q=",2) == 0) {
	  b += 3;
	  b = read_double(q, b);
	} else {
	  ct_begin = b;
	  ct_end = efe;
	}
	b = efe;
      }
      _accepts->insert(std::make_pair(-q, ContentType::FromString(std::string(ct_begin, ct_end - ct_begin))));
      b = ee;
      while(*b == ',' || *b == ' ') b++;
    }
    if(_accepts->size() == 0)
      _accepts->insert(std::make_pair(-1, ContentType::ALL));
    mappedContentType = true;
  }

  void ConnContext::FillContentType() {
    _contentType = ContentType::FromString(Headers()["content-type"]);
    mappedContentType = true;
  }

  void ConnContext::FillPost(){
    mapContents(Payload(), _post);
    mappedPost = true;
  }
  HeaderCollection::value_type& ConnContext::AddRequestHeader(const char* _name, const char* value){
    std::string name(_name);
    std::transform(name.begin(), name.end(), name.begin(), ::tolower);
    auto& newHeader = *_headers.insert( std::make_pair(name,
						      std::string(value)));

    return newHeader;
  }
  void ConnContext::FillCookies(){
    auto range = Headers().equal_range("cookie");
    for(auto it = range.first;it != range.second;it++){
      LOG(Server, Debug) << "Req. cookie: " << it->first << " = " << it->second << std::endl;
      if(it->second.size() == 0) continue;
      const char* n =  &it->second[0];      
      const char *ne, *ve;
      do {
	n = read_to_name(n);
	
	ne = read_name(n);
	ve = read_value(ne);
	_cookies[std::string(n, ne)] = 
	  ne >= ve ? std::string() : std::string(ne+1, ve); 
	n = ve;
	while(*n == ';' || *n == ' ') n++;	
      } while(*n != 0);
    }
    mappedCookies = true;
  }

  std::multimap<double, ContentType::t>& ConnContext::Accepts(){
    if(_accepts == 0){
      FillAccepts();
      assert(_accepts);
    }
    return *_accepts;
  }

  ContentType::t ConnContext::ContentType() {
    if(!mappedContentType){
      this->FillContentType();
      assert(mappedContentType);
    }
    return _contentType;
  }
  std::string& ConnContext::Payload() {
    if(!mappedPayload){
      this->FillPayload();
      assert(mappedPayload);
    }
    return _payload;
  }

  HeaderCollection& ConnContext::Headers(){
    if(!mappedHeaders){
      this->FillHeaders();
      assert(mappedHeaders);
    }
    return _headers;
  }

  CookieCollection& ConnContext::Cookies(){
    if(!mappedCookies){
      this->FillCookies();
      assert(mappedCookies);
    }
    return _cookies;
  }

  QueryStringCollection& ConnContext::QueryString() {
    if(!mappedQs){
      this->FillQueryString();
      assert(mappedQs);
    }
    return _qs;
  }

  PostCollection& ConnContext::Post() {
    if(!mappedPost){
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
    return *this << Header("Set-Cookie", cookie.first + "=" + cookie.second);
  }

  Response& Response::operator <<(const rikitiki::Header& header){   
    headers.push_back(header);
    return *this;
  }

  ConnContext& ConnContext::operator<<(std::function<void(std::ostream&)> f){
    handled = true;
    f(response.response);
    return *this;
  }

  ConnContext& operator>>(ConnContext& ctx, std::string& t){
    t = ctx.Payload();
    return ctx;
  }

  ConnContext::ConnContext(Server* _server) : ConnContext() { server = _server; }
  ConnContext::ConnContext() : _method(ANY), 
			       handled(false), 
			       server(NULL), 
			       mappedPost(false), 
			       mappedQs(false), 
			       mappedHeaders(false), 
			       mappedCookies(false), 
			       mappedPayload(false),
			       mappedContentType(false),
			       _accepts(0) {}

  ConnContext::~ConnContext(){
    delete _accepts; 
  }
  
#define MATCH_METHOD_ENUM(eval)	do{if(strcmp(method, #eval) == 0) return ConnContext::eval;}while(false);
				     
  ConnContext::Method strToMethod(const char* method){
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
  void mapQueryString(const char* _qs, std::map<std::string, std::string>& qs){
    if(_qs == NULL) return;
    std::string name;
    std::string* val = 0;
    bool nameMode = true;
    while(*_qs != '\0'){
      if(nameMode){
	if(*_qs == '='){
	  nameMode = false;
	  val = &qs[name];
	} else if(*_qs == '&'){
	  qs[name] = "";
	  name.clear();
	} else {
	  name += *_qs;
	}
      } else {
	if(*_qs == '&'){
	  nameMode = true;
	  name.clear();
	} else {
	  *val += *_qs;
	}
      }
      _qs++;
    }
  }
  void unescapeString(char* s, char* e, std::string& rtn) {
       rtn.reserve(e - s);
       for (s; s < e; s++) {
            if (*s == '%' && s + 2 < e) {
                 s++;
                 char z = s[2];
                 s[2] = 0; // most we can read is 2
                 rtn.push_back((char)strtol(s, 0, 16));
                 s[2] = z;
                 s++;
            }
            else {
                 rtn.push_back(*s);
            }
       }
  }
  void mapContents(std::string& raw_content, PostCollection& post){
       if (raw_content.size() == 0)
            return;
    if(raw_content.back() != '&')
      raw_content.push_back('&');
    
    std::replace(raw_content.begin(), raw_content.end(), '+', ' ');
    auto l_it = raw_content.begin();
    std::string name, value;
    for(auto it = raw_content.begin(); it != raw_content.end();it++){
      switch(*it){
      case '=': 
	name = std::string(l_it, it);	  
	l_it = it+1;
	break; 
      case '&': 
        
	#ifdef _MSC_VER
           std::string value;
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
