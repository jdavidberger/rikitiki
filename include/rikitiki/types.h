#pragma once

#include <string>
#include <mxcomp/utf.h>

#define RT_STRING_LITERAL
namespace rikitiki {
    typedef std::stringstream stringstream;
    typedef std::string string;

    static inline string to_rt_string(const string& s) { return s; }
    static inline string to_rt_string(const std::wstring& s) { return mxcomp::utf::convert(s); }

    static inline std::wstring from_rt_string(const string& s) { return mxcomp::utf::convert(s); }
    static inline std::wstring from_rt_string(const std::wstring& s) { return s; }

    static inline std::string to_string(const string& s) { return s; }
    static inline std::string to_string(const std::wstring& s) { return mxcomp::utf::convert(s); }

    static inline std::wstring to_wstring(const std::wstring& s) { return s; }
    static inline std::wstring to_wstring(const std::string& s) { return mxcomp::utf::convert(s); }
}
