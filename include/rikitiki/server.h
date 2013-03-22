/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#pragma once 

#include <rikitiki/config.h>
#include <vector>
#include <sstream>
#include "content_types.h"

#include <mxcomp/macros.h>
#include <mxcomp/tuple_ext.h>
#include <tuple>

#include <rikitiki/log/log>
#include <rikitiki/http_statuses.h>

#if RT_USE_CTEMPLATE
#include <rikitiki/ctemplate/templatePreprocessor.h>
#endif

namespace rikitiki {
  class ConnContext;

  /**
     Base handler class. These are checked in order whenever there is a request. 
   */ 
  struct Handler {
    virtual bool Handle(ConnContext& ctx) = 0;
    virtual bool visible() const = 0;
    virtual std::string name() const = 0;
    virtual std::string desc() const;
    virtual ~Handler(); 
  };

  /** Thrown from within handlers to immediately stop handler execution.    
      Note that throwing an exception will treat the request as handled, by design. 
   */
  struct HandlerException {
  HandlerException() : status(0){}
  HandlerException(const HttpStatus& s) : status(&s){}
    /** Optionally specify the status to return.
	If no status is set, Internal_Server_Error is returned. 
     */
    const HttpStatus* status; 
  };
  
  
  /**\brief The main interface file between modules and a given server
     Server is the rikitiki component that stores handlers and interfaces with the 
     choosen server module to fire them off. It also is the object that provides
     information about the current server -- IP, Port, etc. 
  */
  class Server {
  protected:
  public:
    std::vector<Handler*> handlers;
    virtual ~Server();

    #ifdef RT_USE_CTEMPLATE
    std::vector<ctemplates::TemplatePreprocessor*> templatePreprocessors;
    void AddPreprocessor( rikitiki::ctemplates::TemplatePreprocessor*);
    #endif

    typedef bool (*handle_t)(ConnContext& ctx);
 
    bool Handle(ConnContext& ctx);

    void AddHandler( Handler& handler);
    void AddHandler( Handler* handler);
    void AddHandler( handle_t handler );

    template <typename T>
      void Register( T& t ){
      t.Register(*this);
    }
  };

}
