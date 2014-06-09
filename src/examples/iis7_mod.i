/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */
#include <rikitiki/iis7/iis7.h>
#include "${imodule}.h"
using namespace rikitiki::examples;
using namespace rikitiki::iis7;

//Note -- __declexport here doesn't propertly remove the name mangling. 
#pragma comment(linker, "/EXPORT:RegisterModule=?RegisterModule" "@@" "YAJKPEAVIHttpModuleRegistrationInfo" "@@" "PEAVIHttpServer" "@@@" "Z")

HRESULT __stdcall RegisterModule(
     DWORD                           dwServerVersion,
     IHttpModuleRegistrationInfo *   pModuleInfo,
     IHttpServer *                   pHttpServer
     )
{     
     auto pFactory = rikitiki::iis7::CreateServer(dwServerVersion, pModuleInfo, pHttpServer);
     ${modstruct}  module;
     pFactory->Register(module);
     return S_OK;
}
