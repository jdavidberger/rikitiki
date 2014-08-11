#pragma once

#include <string>
#include <map>
#include <vector>

namespace rikitiki {
     typedef std::pair<std::wstring, std::wstring> wstringpair;

     /**
     Things parsed out of the response, ala forms
     We can't just typedef it since we want to pass it around with stream operators
     */
     struct PostContent : public wstringpair {
          PostContent(const std::wstring& name, const std::wstring& value);
     };

     /**
     Cookies are key value pairs too.
     TODO: Add expiration, domain, etc
     */
     struct Cookie : public wstringpair {
          Cookie(const std::wstring& nameAndValue);
          Cookie(const std::wstring& name, const std::wstring& value,
               const std::wstring& Domain = L"", const std::wstring& Path = L"/",
               const std::wstring& Expires = L"", bool secure = false, bool httpOnly = false);
     };

     /**
     Headers are just string pairs
     We can't just typedef it since we want to pass it around with stream operators
     */
     struct Header : public wstringpair {
          Header(const std::wstring& name, const std::wstring& value);
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

     struct QueryStringCollection : public std::map<std::wstring, std::wstring> {
          void FromQueryString(const std::wstring&);
          void FromQueryString(const std::string&);
     };

     typedef multimap<std::wstring, std::wstring> PostCollection;

     class CookieCollection : public std::map<std::wstring, std::wstring> {};
     class HeaderCollection : public std::map < std::wstring, std::wstring > {

     public:
          void Add(const std::string& name, const std::string& value);
          void Set(const std::string& name, const std::string& value);
          void Add(const std::wstring& name, const std::wstring& value);
          void Set(const std::wstring& name, const std::wstring& value);
          void Set(const std::wstring& name, size_t value);

          const std::wstring* Get(const std::wstring& name) const;
          const std::vector<std::wstring> GetList(const std::wstring& name) const {
               std::vector<std::wstring> rtn;
               const std::wstring* csv = Get(name);
               if (csv == 0)
                    return rtn;
               const wchar_t* buffer = csv->data();
               const wchar_t* n = 0;
               do {
                    n = wcschr(buffer, L',');
                    if (n == 0) {
                         rtn.push_back(buffer); 
                    }
                    else {
                         auto end = n;
                         while (*(end - 1) == ' ') end--;
                         rtn.push_back(std::wstring(buffer, end));
                         buffer = n + 1;
                         while (*buffer == ' ') buffer++;
                    }
               } while (n != 0);
               
               return rtn;
          }

          template <typename T>
          const T Get(const std::wstring& name, T def = T()) const {
               auto v = Get(name);
               T rtn = def;
               std::wstringstream wss;
               if (v) {
                    wss << *v;
                    wss >> rtn;
               }
               return rtn;
          }
     };
}