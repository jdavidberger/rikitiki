/* Copyright (C) 2012-2013 Justin Berger
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#pragma once 
#include <string>
#include <map>
#include <rikitiki/server.h>

namespace rikitiki {
     struct StaticContentHandler : public Handler {
          std::string prefix;
          std::string path;
          std::map<std::string, std::string> mime_types;
          StaticContentHandler(const std::string& prefix, const std::string& path);
          virtual bool Handle(ConnContextRef ctx) OVERRIDE;
          virtual bool CanHandle(Request& ctx) OVERRIDE;
          virtual bool visible() const;
          virtual std::wstring name() const;
          virtual ~StaticContentHandler();
     };
}
