#pragma once

#include <rikitiki\http\Message.h>
#include <rikitiki\http\Request.h>
#include <rikitiki\http\Response.h>

namespace rikitiki {
     namespace http {
          namespace helpers {
               /**
               Maps compound types to 'const T&', others to 'T'.
               */
               template <typename T, bool Enable = std::is_compound<T>::value && std::is_enum<T>::value == false>
               struct sane_ref_type{
                    typedef T type;
               };

               /**
               Maps compound types to 'const T&', others to 'T'.
               */
               template <typename T>
               struct sane_ref_type<T, true>{
                    typedef const T& type;
               };


#define BufferMemberNoSetter(T, name, access) \
                   private: mutable Buffered_<std::remove_reference<T>::type> _ ## name; \
                   public: \
                         virtual T name() access OVERRIDE { \
                              return _ ## name .Get(this, &thisType::Fill ## name); \
                                                                      };   \
                    protected: \
                         virtual void Fill ## name(std::remove_reference<T>::type&) const 

#define BufferMember(T, name) \
                    public: \
                         virtual void Set ## name(sane_ref_type< T >::type v) OVERRIDE { \
                              _ ## name . data = v; \
                              _ ## name . isInBuffer = true; \
                                                       } \
                         BufferMemberNoSetter(T, name, const)

#define BufferMemberPure(type, name) BufferMember(type, name) = 0


               template <class T> struct Buffered_ {
                    bool isInBuffer = false;
                    T data;
                    template <typename S, typename F>
                    T& Get(S* s, F f) {
                         if (!isInBuffer) {
                              (s->*f)(data);
                              isInBuffer = true;
                         }
                         return data;
                    }
               };
#define NON_CONST 
               class BufferedMessage : public virtual Message {
                    typedef BufferedMessage thisType;
                    rikitiki::ByteStream body;
                    BufferMember(size_t, ContentLength);
                    BufferMember(Encoding::t, TransferEncoding);
                    BufferMember(ContentType::t, ContentType);
                    //BufferMemberNoSetter(ByteStream&, Body, NON_CONST) = 0;
                    BufferMemberNoSetter(HeaderCollection&, Headers, NON_CONST) = 0;
                    BufferMemberNoSetter(CookieCollection&, Cookies, NON_CONST);     
               public:
                    rikitiki::ByteStream& Body() { return body; }
               };

               class BufferedRequest : public BufferedMessage, public Request {
                    typedef BufferedRequest thisType;
                    BufferMemberPure(RequestMethod::t, RequestMethod);
                    BufferMemberNoSetter(QueryStringCollection&, QueryString, NON_CONST) = 0;
               };
          }
     }
}