// -*-c++-*-
/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

template <class T>
ConnContext& ConnContext::operator <<(const T& obj){
  handled = true;
  response.status = true;	
  response << obj;
  return *this;
}
