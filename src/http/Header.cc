#include <string>
#include <map>
#include <vector>
#include <rikitiki\http\Header.h>
#include <assert.h>
#include <mxcomp\useful_macros.h>
#include <sstream>
#include <locale>
#include <codecvt>

namespace rikitiki {

     Cookie::Cookie(const std::wstring&) {
          assert(false);
     }

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

     void HeaderCollection::Add(const std::wstring& name, const std::wstring& value) {
          std::wstring& current = (*this)[name];
          if (current.length() > 0) {
               current += L", ";
          }
          current += value;
     }

     void HeaderCollection::Add(const std::string& name, const std::string& value) {
          std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
          Add(converter.from_bytes(name.data()), converter.from_bytes(value.data()));
     }
     void HeaderCollection::Set(const std::string& name, const std::string& value) {
          std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
          Set(converter.from_bytes(name.data()), converter.from_bytes(value.data()));
     }

     void HeaderCollection::Set(const std::wstring& name, const std::wstring& value) {
          (*this)[name] = value;          
     }
     void HeaderCollection::Set(const std::wstring& name, size_t value) {
          std::wstringstream ss; 
          ss << value;
          Set(name, ss.str());
     }

     const std::wstring* HeaderCollection::Get(const std::wstring& name) const {
          auto el = this->find(name);
          if (el == this->end())
               return 0;
          return &(*el).second;
     }
     void QueryStringCollection::FromQueryString(const std::string& qs) {
          std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
          FromQueryString(converter.from_bytes(&qs[0]));
     }
     void QueryStringCollection::FromQueryString(const std::wstring& qs) {
          const wchar_t* _qs = &qs[0];
          std::wstring name;
          std::wstring* val = 0;
          bool nameMode = true;
          while (*_qs != 0){
               if (nameMode){
                    if (*_qs == L'='){
                         nameMode = false;
                         val = &(*this)[name];
                    }
                    else if (*_qs == L'&'){
                         (*this)[name] = L"";
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
}