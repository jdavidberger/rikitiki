#pragma once 

#include <string>
#include <map>
#include "content_types.h"
#include <vector>
#include <sstream>
#include <mxcomp\useful_macros.h>
#include <mxcomp\cont_stringbuf.h>
#include <assert.h>
#include <rikitiki\http\Enums.h>
#include <rikitiki\http\parsing\BufferedReader.h>
#include <rikitiki\http\parsing\MessageParserState.h>

#pragma warning (disable: 4265)
#include <condition_variable>
#pragma warning (default: 4265)

namespace rikitiki {
     using namespace http::parsing;

     class Cookie;
     class Header;
     class HeaderCollection;
     class CookieCollection;

     // http://www.w3.org/Protocols/rfc2616/rfc2616-sec4.html#sec4
     typedef std::basic_stringstream<char> ByteStream;
     class Message {
     public:          
          virtual ~Message() {}

          virtual HeaderCollection& Headers() = 0;
          virtual ByteStream& Body() = 0;
          virtual CookieCollection& Cookies() = 0;

          virtual size_t ContentLength() const;
          virtual Encoding::t TransferEncoding() const;          
          virtual ContentType::t ContentType() const;
          virtual std::wstring Startline() const = 0; 

          virtual void SetContentLength(size_t);
          virtual void SetTransferEncoding(Encoding::t);
          virtual void SetContentType(ContentType::t t);
          virtual void SetStartline(const std::wstring&) = 0;          

          const HeaderCollection& Headers() const;
          const ByteStream& Body() const;
          const CookieCollection& Cookies() const;

          template <class T> auto operator <<(const T& obj) -> decltype(std::declval<std::stringstream>() << obj, std::declval<Message&>())
          {
               //std::lock_guard<std::mutex> lock(payloadWrite);
               Body() << obj;
               assert(Body().good());
               return *this;
          }
          virtual Message& operator <<(rikitiki::ContentType::t t);
          virtual Message& operator <<(const rikitiki::Cookie& t);
          virtual Message& operator <<(const rikitiki::Header& t);
     };

     std::ostream&  operator<< (std::ostream&,  const Message&);
     std::wostream& operator<< (std::wostream&, const Message&);

     class MessageListener {
     protected:
          virtual void OnStateChange(MessageState::type) { }
          virtual void OnStartLine(const std::wstring&) {}
          virtual void OnHeader(const std::wstring&, const  std::wstring&) {}
          virtual void OnBodyData(const char*, size_t) {}
          virtual void OnChunkedData(const char* d, size_t len) {
               OnBodyData(d, len);
          }
     public:
          virtual ~MessageListener(){}
          virtual bool OnData(const char*, size_t) = 0; 
     };

     struct MessageBroadcast : public MessageListener {
          std::vector < MessageListener* > listeners;
          virtual ~MessageBroadcast();

          virtual void OnStateChange(MessageState::type);
          virtual void OnStartLine(std::wstring&);
          virtual void OnHeader(std::wstring&, std::wstring&);
          virtual void OnData(char*, size_t);
          virtual void OnChunkedData(char*, size_t);
     };

     class IMessage : BufferedReader, public MessageListener, public virtual Message {
     protected:                    
          http::parsing::MessageParserState currentState;

          void SetState(MessageState::type newState) { currentState = newState; }
          virtual bool OnBufferedData(const char* data, std::size_t len) OVERRIDE;
          void UpdateBufferState();
     public:
          bool OnData(const char* data, size_t len) OVERRIDE{
               return BufferedReader::OnData(data, len);
          }
     };

     template <class T>
     struct IMessage_ :  public IMessage, public T {          
          std::condition_variable stateConditionVariable;
          std::mutex stateMutex; 

          virtual void OnStateChange(MessageState::type newState) { 
               currentState = newState;
               stateConditionVariable.notify_all();
          }
          virtual void OnStartLine(const std::wstring& sl) {
               SetStartline(sl);
          }
          virtual void OnHeader(const std::wstring& name, const  std::wstring& value) {
               T::Headers().Add(name, value);
          }          
          virtual void OnBodyData(const char* data, size_t len) {
               T::Body().write(data, (std::streamsize)len);
          }
          virtual HeaderCollection& Headers() {
               while (currentState.streamState <= MessageState::HEADERS) {
                    std::unique_lock<std::mutex> l(stateMutex);
                    stateConditionVariable.wait(l);
               }
               return T::Headers();
          }          
          virtual ByteStream& Body() {
               while (currentState.streamState < MessageState::FINISHED) {
                    std::unique_lock<std::mutex> l(stateMutex);
                    stateConditionVariable.wait(l);
               }
               return T::Body();
          }
     };     

     struct InvalidStateException : public std::exception {
          InvalidStateException(const std::string& msg);
     };

     struct OMessage {
     protected:          
          MessageParserState streamState = MessageParserState(MessageState::START_LINE);          

          Encoding::t encoding = Encoding::UNKNOWN;
          size_t contentSize = (size_t)-1;

          virtual void SetState(MessageState::type st);
          virtual void OnStateChange(MessageState::type) { }
          virtual void HeadersDone() {}          
     public:          
          virtual ~OMessage() {};
          virtual void Done() {}
          virtual size_t WritePayloadData(const char*, size_t) = 0;
          virtual void WriteHeader(const Header&) = 0;

          virtual OMessage& operator <<(rikitiki::ContentType::t t);
          virtual OMessage& operator <<(const rikitiki::Cookie& t);
          virtual OMessage& operator <<(const rikitiki::Header& t);
          
          size_t WritePayloadData(const std::string& buff);          
     };

     struct OMessageWriter : public virtual OMessage {
     protected:
          
          std::string buffer;

          virtual size_t WriteStartLine() = 0;
          virtual size_t WriteData(const char*, size_t) = 0;
          size_t WriteData(const wchar_t*, size_t);
          size_t WriteData(const std::wstring&);
          size_t WriteData(const std::string&);
     public:
          virtual void Done() OVERRIDE;
          virtual ~OMessageWriter() {};
          virtual void WriteHeader(const Header&) OVERRIDE;
          virtual size_t WritePayloadData(const char*, size_t) OVERRIDE;
          size_t WritePayloadData(const std::string& buff);
     };

     template <class T> auto operator <<(OMessage& out, const T& obj) -> decltype(std::declval<std::stringstream>() << obj, std::declval<OMessage&>())
     {
          std::stringstream ss;           
          auto buff = mxcomp::attachContinousBuffer(ss);
          ss << obj;
          out.WriteData(buff->data(), buff->length());          
          return out;
     }
}