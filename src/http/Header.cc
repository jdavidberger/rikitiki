#include <rikitiki/http/Header.h>

#include <mxcomp/utf.h>
#include <mxcomp/useful_macros.h>

#include <string>
#include <map>
#include <vector>
#include <assert.h>
#include <sstream>

namespace rikitiki {

     Cookie::Cookie(const rikitiki::string&) {
          assert(false);
     }

     Cookie::Cookie(const rikitiki::string& name, const rikitiki::string& value,
          const rikitiki::string& Domain, const rikitiki::string& Path,
          const rikitiki::string& Expires, bool secure, bool httpOnly) {
          first = name;
          second = value + RT_STRING_LITERAL"; ";
          //    if(Domain.size())
          second += RT_STRING_LITERAL"Domain=" + Domain + RT_STRING_LITERAL"; ";
          if (Path.size())
               second += RT_STRING_LITERAL"Path=" + Path + RT_STRING_LITERAL"; ";
          if (Expires.size())
               second += RT_STRING_LITERAL"Expires=" + Expires + RT_STRING_LITERAL"; ";
          if (secure)
               second += RT_STRING_LITERAL"Secure; ";
          if (httpOnly)
               second += RT_STRING_LITERAL"HttpOnly";
     }

     void HeaderCollection::Add(const std::string& name, const std::string& value) {
          auto& current = (*this)[name];
          if (current.length() > 0) {
               current += RT_STRING_LITERAL", ";
          }
          current += value;
     }

     void HeaderCollection::Add(const std::wstring& name, const std::wstring& value) {
          Add(mxcomp::utf::convert(name.data()), mxcomp::utf::convert(value.data()));
     }
     void HeaderCollection::Set(const std::string& name, const std::string& value) {
          Set(mxcomp::utf::convert(name.data()), mxcomp::utf::convert(value.data()));
     }

     void HeaderCollection::Set(const std::wstring& name, const std::wstring& value) {
          (*this)[ rikitiki::to_rt_string(name)] = rikitiki::to_rt_string(value);
     }
     void HeaderCollection::Set(const std::wstring& name, size_t value) {
          rikitiki::stringstream ss;
          ss << value;
          Set(rikitiki::to_rt_string(name), ss.str());
     }
    void HeaderCollection::Set(const std::string& name, size_t value) {
         rikitiki::stringstream ss;
         ss << value;
         Set(rikitiki::to_rt_string(name), ss.str());
    }

     const rikitiki::string* HeaderCollection::Get(const rikitiki::string& name) const {
          auto el = this->find(name);
          if (el == this->end())
               return 0;
          return &(*el).second;
     }
     void QueryStringCollection::FromQueryString(const std::string& qs) {
          FromQueryString(mxcomp::utf::convert(&qs[0]));
     }
     void QueryStringCollection::FromQueryString(const std::wstring& qs) {
          const wchar_t* _qs = &qs[0];
          rikitiki::string name;
          rikitiki::string* val = 0;
          bool nameMode = true;
          while (*_qs != 0){
               if (nameMode){
                    if (*_qs == RT_STRING_LITERAL'='){
                         nameMode = false;
                         val = &(*this)[name];
                    }
                    else if (*_qs == RT_STRING_LITERAL'&'){
                         (*this)[name] = RT_STRING_LITERAL"";
                         name.clear();
                    }
                    else {
                         name += *_qs;
                    }
               }
               else {
                    if (*_qs == RT_STRING_LITERAL'&'){
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

    bool QueryStringCollection::hasValue(const rikitiki::string &name, rikitiki::string &value) {
         auto find = this->find(name);
         if(find == this->end())
              return false;
         value = find->second;
         return true;
    }
}
