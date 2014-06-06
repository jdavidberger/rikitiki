/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */
#include <rikitiki/cef/cef>
#include "${imodule}.h"

using namespace rikitiki::examples;
using namespace rikitiki::cef;

#include <list>
class DemoClient : public CefClient {
private:
     // Include the default reference counting implementation.
     IMPLEMENT_REFCOUNTING(DemoClient);
};
class DemoApp : public CefApp,
     public CefBrowserProcessHandler {
public:
  DemoApp() {}

     // CefApp methods:
     virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE{ return this; }

     // CefBrowserProcessHandler methods:
     virtual void OnContextInitialized() OVERRIDE {
       CefWindowInfo window_info;

       ${modstruct} module;
       auto server = new CefInternalServer;
       CefRegisterSchemeHandlerFactory("http", "app", server);
       server->Register(module);
              
#if defined(OS_WIN)
       // On Windows we need to specify certain flags that will be passed to
       // CreateWindowEx().
       window_info.SetAsPopup(NULL, "${modstruct} demo");
#endif

       // Specify CEF browser settings here.
       CefBrowserSettings browser_settings;

       CefRefPtr<CefCommandLine> command_line =
	 CefCommandLine::GetGlobalCommandLine();

       auto url = command_line->GetSwitchValue("url");
       if (url.empty())
			url = "http://app/";
       // Create the first browser window.
                CefBrowserHost::CreateBrowser(window_info, new DemoClient, url,
				     browser_settings, NULL);       
     }

 private:
     // Include the default reference counting implementation.
     IMPLEMENT_REFCOUNTING(DemoApp);
};


int APIENTRY wWinMain(HINSTANCE hInstance,
     HINSTANCE hPrevInstance,
     LPTSTR    lpCmdLine,
     int       nCmdShow) {
	 
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);
	UNREFERENCED_PARAMETER(hPrevInstance);

     // Provide CEF with command-line arguments.
     CefMainArgs main_args(hInstance);

     // SimpleApp implements application-level callbacks. It will create the first
     // browser instance in OnContextInitialized() after CEF has initialized.
     CefRefPtr<DemoApp> app(new DemoApp);

     // CEF applications have multiple sub-processes (render, plugin, GPU, etc)
     // that share the same executable. This function checks the command-line and,
     // if this is a sub-process, executes the appropriate logic.
     int exit_code = CefExecuteProcess(main_args, app.get());
     if (exit_code >= 0) {
          // The sub-process has completed so return here.
          return exit_code;
     }

     // Specify CEF global settings here.
     CefSettings settings;

     // Initialize CEF.
     CefInitialize(main_args, settings, app.get());

     // Run the CEF message loop. This will block until CefQuitMessageLoop() is
     // called.
     CefRunMessageLoop();

     // Shut down CEF.
     CefShutdown();

     return 0;
}

