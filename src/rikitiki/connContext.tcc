// -*-c++-*-
/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

template <class T>
ConnContext& ConnContext::operator <<(const T& obj){
  handled = true;
  response << obj;
  return *this;
}

template <typename T1, typename T2>
T2& rikitiki::multimap<T1,T2>::operator[](const T1& key){
  typename rikitiki::multimap<T1,T2>::iterator it = this->find(key);
  if(it != this->end() )
    return it->second;
  it = this->insert(std::pair<T1, T2>(key, T2()));
  return it->second;
}

