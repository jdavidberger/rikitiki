/* Copyright (C) 2012-2013 Justin Berger
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include <mxcomp/log.h>
#include <rikitiki/connContext.h>
#include <algorithm>
#include <mxcomp/utf.h>
#include <assert.h>
#include <cstring>

namespace rikitiki {
     void type_conversion_error(ConnContext& ctx, void** handlers){
          rikitiki::string accepts;
          for (auto _type = 0; _type < (int)ContentType::MAX; _type++)
               if (handlers[_type]){
               if (accepts.size())
                    accepts += RT_STRING_LITERAL", ";
               accepts += ContentType::ToString((ContentType::t)_type);
               }
          ctx << Header(RT_STRING_LITERAL"Accept", accepts);
          throw rikitiki::HandlerException(HttpStatus::Not_Acceptable);
     }

     std::ostream& operator <<(std::ostream& response, const wchar_t* obj)
     {
       return response << mxcomp::utf::convert(obj);
     }
     std::ostream& operator <<(std::ostream& response, const rikitiki::string& obj) {
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

    ConnContext &ConnContext::operator<<(const std::string & s) {
        this->Response.WritePayloadData(s);
        return *this;
    }

    ConnContext &ConnContext::operator<<(const char * s) {
        this->Response.WritePayloadData(s);
        return *this;
    }

    ConnContext &ConnContext::operator<<(rikitiki::ContentType::t t) {
        this->Response << t;
        return *this;
    }

    void unescapeString(rikitiki::string::value_type * s, rikitiki::string::value_type * e, rikitiki::string& rtn) {
          assert(e > s);
          rtn.reserve((rikitiki::string::size_type)(e - s));
          for (; s < e; s++) {
               if (*s == '%' && s + 2 < e) {
                    s++;
                    wchar_t z = s[2];
                    s[2] = 0; // most we can read is 2
                    rtn.push_back((rikitiki::string::value_type )strtol(s, 0, 16));
                    s[2] = z;
                    s++;
               }
               else {
                    rtn.push_back(*s);
               }
          }
     }

     void mapContents(ByteStream& raw_content_stream, PostCollection& post){
       rikitiki::string raw_content = rikitiki::to_rt_string( raw_content_stream.str() );

          if (raw_content.size() == 0)
               return;
          if (raw_content.back() != L'&')
               raw_content.push_back(L'&');

          std::replace(raw_content.begin(), raw_content.end(), L'+', L' ');
          auto l_it = raw_content.begin();
          rikitiki::string name;
          for (auto it = raw_content.begin(); it != raw_content.end(); it++){
               switch (*it){
               case L'=':
                    name = rikitiki::string(l_it, it);
                    l_it = it + 1;
                    break;
               case L'&':

                    rikitiki::string value;
                    unescapeString(&*l_it, &*it, value);
                    post.insert(PostContent(name, value));

                    l_it = it + 1;
                    break;
               }
          }
     }
}
