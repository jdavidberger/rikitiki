// -*-c++-*-
/* Copyright (C) 2012-2013 Justin Berger
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */

namespace rikitiki {

  /*
     template <class T>
     typename std::enable_if<  std::is_class<typename valid_conversions<T>::In>::value,
          ConnContext&>::type ConnContext::operator <<(T& obj) {
       auto rType = Response.ContentType;
          auto& handlers = valid_conversions<T>::Out::Instance().handlers;

          // We should always respect whatever the user set as the response type
          if (rType == ContentType::DEFAULT){
               auto& accepts = Accepts();
               for (auto t : accepts){
                    int i = -1;
                    if (t.second == ContentType::ALL){
                         i = 0;
                         while (i < (int)handlers.size() && handlers[i] == NULL) i++;
                    }
                    else {
                         i = (int)t.second;
                    }
                    if (i < (int)handlers.size()){
                         auto handler = handlers[i];
                         if (handler){
                              *this << (ContentType::t)i;
                              handler(*this, obj);
                              return *this;
                         }
                    }
               }
          }
          else {
               auto& handler = handlers[rType];
               if (handler){
                    handler(*this, obj);
                    return *this;
               }
          }
          type_conversion_error(*this, (void**)&handlers[0]);
          // Never actually hit since throw_conversion_error always throws
          return *this;
     }

     template <class T>
     auto ConnContext::operator >>(T&s) -> decltype(valid_conversions<T>::In::Instance(), instance_of<ConnContext>::value){
          auto cType = ContentType();
          auto& handlers = valid_conversions<T>::In::Instance().handlers;
          if (cType == ContentType::DEFAULT){
               type_conversion_error(*this, (void**)&handlers[0]);
          }

          assert((int)cType < (int)ContentType::MAX);
          auto& handler = handlers[(int)cType];

          if (!handler) {
               type_conversion_error(*this, (void**)&handlers[0]);
          }

          try {
               handler(*this, s);
          }
          catch (std::exception& e) {
               *this << "Couldn't read content due to parser exception: "
                    << e.what();
               throw HandlerException(HttpStatus::Bad_Request);
          }
          return *this;
     }

     template <typename T1, typename T2>
     T2& rikitiki::multimap<T1, T2>::operator[](const T1& key){
          typename rikitiki::multimap<T1, T2>::iterator it = this->find(key);
          if (it != this->end())
               return it->second;
          it = this->insert(std::pair<T1, T2>(key, T2()));
          return it->second;
     }
  */
}
