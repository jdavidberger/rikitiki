// -*-c++-*-
/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

namespace rikitiki {
  namespace apache {
    template <typename T>
      static int handler(request_rec *r){
      static T* t = 0;    
      static ApacheServer server(r->server);
      
      if(t == 0){
	t = new T();
	server.Register(*t);
      } else {
	server.server = r->server;
      }
      
      ApacheConnContext ctx(&server, r);
      bool handled = server.Handle(ctx);
      return handled ? DONE : DECLINED;
    }
  } 
}
