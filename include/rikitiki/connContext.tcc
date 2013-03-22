// -*-c++-*-
/* Copyright (C) 2012-2013 Justin Berger 
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

template <class T>
auto ConnContext::operator <<(const T& obj)  -> decltype(instance_of<Response>::value << obj, 
							 (ConnContext&)*(ConnContext*)0) {
  handled = true;
  response << obj;
  return *this;
}

static void type_conversion_error(ConnContext& ctx, void** handlers){
  std::string accepts;
  for(auto _type = 0; _type < (int)ContentType::MAX; _type++)
    if(handlers[_type]){
      if(accepts.size())
	 accepts += ", ";
      accepts += ContentType::ToString((ContentType::t)_type);
    }
  ctx << Header("Accept", accepts);
  throw HandlerException(HttpStatus::Not_Acceptable);
}

template <class T> 
auto ConnContext::operator <<(T& obj) -> decltype(valid_conversions<T>::Out::Instance(), instance_of<ConnContext>::value){
  auto rType = response.ResponseType;
  auto& handlers = valid_conversions<T>::Out::Instance().handlers;
  

  // We should always respect whatever the user set as the response type
  if(rType == ContentType::DEFAULT){ 
    auto& accepts = Accepts();
    for(auto t : accepts){
      int i = -1;      
      if(t.second == ContentType::ALL){
	i = 0;
	while(handlers[i] == NULL && i < (int)handlers.size()) i++;
      } else {
	i = (int)t.second;
      }
      if(i < (int)handlers.size()){	  
	auto handler = handlers[i];
	if(handler){
	  *this << (ContentType::t)i;
	  handler(*this, obj);
	  return *this;
	}
      }
    }
  } else {
    auto& handler = handlers[rType];
    if(handler){
      handler(*this, obj);
      return *this;
    }
  }
  type_conversion_error(*this, (void**)&handlers[0]);
  // Never actually hit since throw_conversion_error always throws
  return *this;
}
  
template <class T> 
auto ConnContext::operator >>(T&s) -> decltype(valid_conversions<T>::In ::Instance(), instance_of<ConnContext>::value){
  auto cType = ContentType();
  auto& handlers = valid_conversions<T>::In::Instance().handlers;
  if(cType == ContentType::DEFAULT){
    type_conversion_error(*this, (void**)&handlers[0]);
  }

  assert((int)cType < (int)ContentType::MAX);
  auto& handler = handlers[(int)cType ];

  if(!handler) {
    type_conversion_error(*this, (void**)&handlers[0]);
  }  

  try {
    handler(*this, s);
  } catch (std::exception& e) {
    *this << "Couldn't read content due to parser exception: " 
	 << e.what();
    throw HandlerException(HttpStatus::Bad_Request);
  }
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
