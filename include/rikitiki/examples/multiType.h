/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include <rikitiki/rikitiki>

namespace rikitiki {
  namespace examples {
    /** Example to test out two things:
	- That we can handle multiple types -- simple types, etc. 
	- Show that apply tuples bottoms out in one call. Ditto for modern_scanf.
	  You can do this by running 
	  ~~~~~~~
	  valgrind --tool=callgrind --dump-instr=yes ./multiType 
	  ~~~~~~~
	  and then viewing the resulting call graphs from a url like:
	  http://localhost:5000/mt/test/1.23/1231.2123/10/a/testing
	  
	  Verified to behave as expected in v0.1.38. 
     */
    struct MultiTypeModule {
      void Register(Server& server){
	server.AddHandler( CreateRoute<std::string, float, double, int, char, std::string>::
			   With(this, "/mt/{string}/{floa}/{doubl}/{in}/{ch}/{string}", &MultiTypeModule::zigzag) );
      }
      void zigzag(ConnContext& ctx, const std::string& a, float b, double c, int d, char e, const std::string& f){
	ctx << a << " "
	    << b << " "
	    << c << " "
	    << d << " "
	    << e << " "
	    << f << "\r\n";
      }
    };
  }
}