/* Copyright (C) 2012-2013 Justin Berger
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#pragma once 
#include <string>
#include <map>
#include <rikitiki\rikitiki>

namespace rikitiki {
     struct StaticContentHandler : public Handler {
          std::wstring prefix;
          std::wstring path;
          std::map<std::wstring, std::wstring> mime_types;
          StaticContentHandler(const std::wstring& prefix, const std::wstring& path);
          virtual bool Handle(ConnContextRef ctx) OVERRIDE;
          virtual bool CanHandle(RequestContext& ctx) OVERRIDE;
          virtual bool visible() const;
          virtual std::wstring name() const;
          virtual ~StaticContentHandler();
     };
}