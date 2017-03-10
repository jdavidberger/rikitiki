#pragma once

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <rikitiki/types.h>
#include <cstring>

namespace rikitiki {
     typedef std::pair<rikitiki::string, rikitiki::string> wstringpair;

     /**
     Things parsed out of the response, ala forms
     We can't just typedef it since we want to pass it around with stream operators
     */
     struct PostContent : public wstringpair {
          PostContent(const rikitiki::string& name, const rikitiki::string& value);
     };

     /**
     Cookies are key value pairs too.
     TODO: Add expiration, domain, etc
     */
     struct Cookie : public wstringpair {
          Cookie(const rikitiki::string& nameAndValue);
          Cookie(const rikitiki::string& name, const rikitiki::string& value,
               const rikitiki::string& Domain = RT_STRING_LITERAL"", const rikitiki::string& Path = RT_STRING_LITERAL"/",
               const rikitiki::string& Expires = RT_STRING_LITERAL"", bool secure = false, bool httpOnly = false);
     };

     /**
     Headers are just string pairs
     We can't just typedef it since we want to pass it around with stream operators
     */
     struct Header : public wstringpair {
          Header(const rikitiki::string& name, const rikitiki::string& value);
     };

     /**
     The normal multimap, but with an operator[] defined.
     */
     template <typename T1, typename T2>
     struct multimap : public std::multimap<T1, T2> {
          T2& operator[](const T1&);
          template <typename S>
          bool hasValue(const T1& key, S& rtn) {
               typename rikitiki::multimap<T1, T2>::iterator it = this->find(key);
               if (it != this->end()){
                    rtn = it->second;
                    return true;
               }
               return false;
          }
          void push_back(const wstringpair& value) {
               this->insert(value);
          }
     };

     struct QueryStringCollection : public std::map<rikitiki::string, rikitiki::string> {
          void FromQueryString(const std::wstring&);
          void FromQueryString(const std::string&);
         bool hasValue(const rikitiki::string& name, rikitiki::string& value);
     };

     typedef multimap<rikitiki::string, rikitiki::string> PostCollection;

     class CookieCollection : public std::map<rikitiki::string, rikitiki::string> {};
     class HeaderCollection : public std::map < rikitiki::string, rikitiki::string > {

     public:
          void Add(const std::string& name, const std::string& value);
          void Set(const std::string& name, const std::string& value);
          void Set(const std::string& name, size_t value);

          void Add(const std::wstring& name, const std::wstring& value);
          void Set(const std::wstring& name, const std::wstring& value);
          void Set(const std::wstring& name, size_t value);

          const rikitiki::string* Get(const rikitiki::string& name) const;
          const std::vector<rikitiki::string> GetList(const rikitiki::string& name) const {
               std::vector<rikitiki::string> rtn;
               const rikitiki::string* csv = Get(name);
               if (csv == 0)
                    return rtn;
               auto buffer = csv->data();
               auto n = buffer;
               do {
                    n = strchr(buffer, RT_STRING_LITERAL',');
                    if (n == 0) {
                         rtn.push_back(buffer); 
                    }
                    else {
                         auto end = n;
                         while (*(end - 1) == ' ') end--;
                         rtn.push_back(rikitiki::string(buffer, end));
                         buffer = n + 1;
                         while (*buffer == ' ') buffer++;
                    }
               } while (n != 0);
               
               return rtn;
          }

          template <typename T>
          const T Get(const rikitiki::string& name, T def = T()) const {
               auto v = Get(name);
               T rtn = def;
               rikitiki::stringstream wss;
               if (v) {
                    wss << *v;
                    wss >> rtn;
               }
               return rtn;
          }
     };
}
