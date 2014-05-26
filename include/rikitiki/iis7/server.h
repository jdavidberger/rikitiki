#define _WINSOCKAPI_
// http://social.msdn.microsoft.com/Forums/vstudio/en-US/96202d94-f108-483b-b007-6d3de6e303f4/ntddiwin7sp1-is-missing-from-80-sdkddkverh
#define NTDDI_WIN7SP1                       0x06010100
#pragma warning (disable: 4365 4574 4263 4264 )
#include <windows.h>
#include <sal.h>
#include <httpserv.h>
#pragma warning (default: 4365 4574 4263 4264 )
#include <rikitiki\rikitiki>

namespace rikitiki {
     namespace iis7 {
          Server* CreateServer(DWORD _dwServerVersion, IHttpModuleRegistrationInfo * _pModuleInfo, IHttpServer * _pHttpServer);
     }
}