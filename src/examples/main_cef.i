/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */
#include <rikitiki/cef/cef>
#include "${imodule}.h"

using namespace rikitiki::examples;
using namespace rikitiki::cef;

#include <include/cef_client.h>
#include <include/cef_app.h>
#include <include/cef_client.h>

#include <list>

class DemoHandler : public CefClient,
     public CefDisplayHandler,
     public CefLifeSpanHandler,
     public CefLoadHandler {
 public:
  CefRefPtr<CefInternalServer> rHandler;
 DemoHandler() : rHandler(new CefInternalServer()) {
  ${modstruct} module;
  rHandler->Register(module);
  }
  ~DemoHandler() {}

  // Provide access to the single global instance of this object.
  static DemoHandler* GetInstance();

  // CefClient methods:
  virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE{
    return this;
  }
  virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE{
    return this;
  }
    
  virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE{
    return this;
  }

  virtual CefRefPtr<CefRequestHandler> GetRequestHandler() OVERRIDE{
    return rHandler;
  }

  // CefDisplayHandler methods:
  virtual void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) {}

  // CefLifeSpanHandler methods:
  virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) {};
  virtual bool DoClose(CefRefPtr<CefBrowser> browser) {return false;};
  virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) {};

  // CefLoadHandler methods:
  virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
			   CefRefPtr<CefFrame> frame,
			   ErrorCode errorCode,
			   const CefString& errorText,
			   const CefString& failedUrl) {};

  // Request that all existing browser windows close.
  void CloseAllBrowsers(bool force_close);

     bool IsClosing() const { return is_closing_; }

private:
     // List of existing browser windows. Only accessed on the CEF UI thread.
     typedef std::list<CefRefPtr<CefBrowser> > BrowserList;
     BrowserList browser_list_;

     bool is_closing_;

     // Include the default reference counting implementation.
     IMPLEMENT_REFCOUNTING(DemoHandler);
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
       
#if defined(OS_WIN)
       // On Windows we need to specify certain flags that will be passed to
       // CreateWindowEx().
       window_info.SetAsPopup(NULL, "${modstruct} demo");
#endif

       // SimpleHandler implements browser-level callbacks.
       CefRefPtr<DemoHandler> handler(new DemoHandler());

       // Specify CEF browser settings here.
       CefBrowserSettings browser_settings;

       CefRefPtr<CefCommandLine> command_line =
	 CefCommandLine::GetGlobalCommandLine();

       auto url = command_line->GetSwitchValue("url");

       // Create the first browser window.
       CefBrowserHost::CreateBrowser(window_info, handler.get(), url,
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

     void* sandbox_info = NULL;

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

