/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#pragma once
#include "connContext.h"
#include "../routing.h"
#include <rikitiki/apache/server.h>
#include <apache2/httpd.h>
#include <apache2/http_config.h>

#define RT_APACHE_MODULE(name, init)					\
  namespace rikitiki { namespace apache { namespace name ## module {	\
	static ApacheServer * server = 0;				\
	static int post_config(apr_pool_t *pconf,apr_pool_t *plog,	\
			       apr_pool_t *ptemp,server_rec *s) {	\
	  server = new ApacheServer( s );				\
	  init(server);							\
	  return OK;							\
	}								\
	static int handler(request_rec *r) {				\
	  assert(server);						\
	  bool handled = server->Handle(r);				\
	  return handled ? DONE : DECLINED;				\
	}								\
	static void register_hooks(apr_pool_t * p) {			\
	  ap_hook_post_config( post_config, NULL, NULL, APR_HOOK_MIDDLE); \
	  ap_hook_handler( handler, NULL, NULL, APR_HOOK_MIDDLE);	\
	}								\
      }}} /*Namespaces*/						\
  module AP_MODULE_DECLARE_DATA name ##_module  = {			\
    STANDARD20_MODULE_STUFF,						\
    NULL,								\
    NULL,								\
    NULL,								\
    NULL,								\
    NULL,								\
    rikitiki::apache::name ## module::register_hooks			\
  };									


