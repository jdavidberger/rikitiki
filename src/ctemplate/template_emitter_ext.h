#pragma once
#include <ctemplate/template.h>
#include <sys/types.h>     // for size_t
#include <string>
#include <sstream>

// NOTE: if you are statically linking the template library into your binary
// (rather than using the template .dll), set '/D CTEMPLATE_DLL_DECL='
// as a compiler flag in your project file to turn off the dllimports.
#ifndef CTEMPLATE_DLL_DECL
# define CTEMPLATE_DLL_DECL  __declspec(dllimport)
#endif

namespace ctemplate {
  class StringStreamEmitter : public ExpandEmitter {
    std::stringstream* const outbuf_;
  public:
  StringStreamEmitter(std::stringstream* outbuf) : outbuf_(outbuf) {}
    virtual void Emit(char c) { *outbuf_ << c; }
    virtual void Emit(const std::string& s) { *outbuf_ << s; }
    virtual void Emit(const char* s) { *outbuf_ << s; }
    virtual void Emit(const char* s, size_t slen) { outbuf_->write(s, slen); }
  };

  inline bool ExpandTemplate(const TemplateString& filename, Strip strip,
			     const TemplateDictionaryInterface *dictionary,
			     std::stringstream* output) {
    if(output == NULL) return false;
    StringStreamEmitter e(output);
    return mutable_default_template_cache()->ExpandWithData(filename, strip, dictionary, NULL, &e);
  }
}


