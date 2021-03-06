/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

#include <rikitiki/stream_helpers.h>
#ifndef _MSC_VER
#include <cxxabi.h>
#endif
std::ostream& operator<<(std::ostream& stream, const std::type_info& ti){
#ifndef _MSC_VER
  char buf[1024];
  size_t size=1024;
  int status;
  char* res = abi::__cxa_demangle (ti.name(),
				   buf,
				   &size,
				   &status);
  return stream << res;
#else 
	return stream << ti.name();
#endif
}
