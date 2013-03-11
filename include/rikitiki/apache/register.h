/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#pragma once
#include "connContext.h"
#include "../routing.h"
#include <rikitiki/apache/server.h>

namespace rikitiki {
  namespace apache {
    template <typename T>
      static int handler(request_rec *r);

    template <typename T>
      static void register_hooks(apr_pool_t *p){
      ap_hook_handler( handler<T>, NULL, NULL, APR_HOOK_MIDDLE);
    }
  }
}

#include "register.tcc"

#define RegisterApacheHandler(name, handler_t)		\
  module AP_MODULE_DECLARE_DATA name ##_module  = {	\
    STANDARD20_MODULE_STUFF,				\
    NULL,						\
    NULL,						\
    NULL,						\
    NULL,						\
    NULL,						\
    rikitiki::apache::register_hooks<handler_t>,	\
  };

#define RegisterApacheRoute(name, handler_t)	\
  RegisterApacheHandler(name, handler_t);	
