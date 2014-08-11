#pragma once 
#include <mxcomp\log.h>
#include <rikitiki\http\Message.h>
#include <string>
#include <map>

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

     class MessageListener {
     protected:
          virtual void OnStateChange(MessageState::type) { }
          virtual void OnStartLine(const std::wstring&) {}
          virtual void OnHeader(const std::wstring&, const  std::wstring&) = 0;
          virtual void OnBodyData(const char*, size_t) = 0;
          virtual void OnChunkedData(const char* d, size_t len) {
               OnBodyData(d, len);
          }
     public:
          virtual ~MessageListener(){}
          virtual bool OnData(const char*, size_t) = 0; 
     };

     class IMessage : BufferedReader, public MessageListener, public virtual Message {
     protected:                    
          http::parsing::MessageParserState currentState;

          void SetState(MessageState::type newState) { currentState.streamState = newState; }
          virtual bool OnBufferedData(const char* data, std::size_t len) OVERRIDE;
          void UpdateBufferState();
     public:
          virtual void OnStartLine(const std::wstring&) OVERRIDE;
          virtual void OnHeader(const std::wstring&, const  std::wstring&) OVERRIDE;
          virtual void OnBodyData(const char*, size_t) OVERRIDE;
          bool OnData(const char* data, size_t len) OVERRIDE {
               LOG(Message, Debug) << (void*)this << " incoming: " << std::string(data, len) << std::endl;
               return BufferedReader::OnData(data, len);
          }
     };

     template <class T>
     struct IMessage_ :  public IMessage, public T {          
          std::condition_variable stateConditionVariable;
          std::mutex stateMutex; 

          virtual void OnStateChange(MessageState::type newState) { 
               currentState.streamState = newState;
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
}