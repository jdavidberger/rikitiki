/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */
#pragma once
#include <ctemplate/template.h>
#include <sys/types.h>
#include <string>
#include <sstream>

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


