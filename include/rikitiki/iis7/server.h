#define _WINSOCKAPI_
#include <windows.h>
#include <sal.h>
#include <httpserv.h>
#include <rikitiki\rikitiki>

namespace rikitiki {
     namespace iis7 {
          Server* CreateIIS7Server(DWORD _dwServerVersion, IHttpModuleRegistrationInfo * _pModuleInfo, IHttpServer * _pHttpServer);
     }
}