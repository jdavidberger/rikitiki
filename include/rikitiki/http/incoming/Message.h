#pragma once 

#include <rikitiki/http/Message.h>
#include <mxcomp/useful_macros.h>

#include <rikitiki/http/parsing/BufferedReader.h>
#include <rikitiki/http/parsing/MessageParserState.h>

#pragma warning (disable: 4265)
#include <condition_variable>
#pragma warning (default: 4265)

namespace rikitiki {
     using namespace http::parsing;

     class Header;
     class HeaderCollection;

     class MessageListener {
     protected:
          virtual void OnStateChange(MessageState::type) { }
          virtual void OnStartLine(const rikitiki::string&) {}
          virtual void OnHeader(const rikitiki::string&, const  rikitiki::string&) = 0;
          virtual void OnBodyData(const char*, size_t) = 0;
          virtual void OnChunkedData(const char* d, size_t len);
     public:
          virtual ~MessageListener(){}
          virtual bool OnData(const char*, size_t) = 0; 
     };

     class IMessage : BufferedReader, public MessageListener, public virtual Message {
     protected:                    
          http::parsing::MessageParserState currentState;

          void SetState(MessageState::type newState);
          virtual bool OnBufferedData(const char* data, std::size_t len) OVERRIDE;
          void UpdateBufferState();
     public:
          virtual void OnStartLine(const rikitiki::string&) OVERRIDE;
          virtual void OnHeader(const rikitiki::string&, const  rikitiki::string&) OVERRIDE;
          virtual void OnBodyData(const char*, size_t) OVERRIDE;
          bool OnData(const char* data, size_t len) OVERRIDE;
     };

     template <class T>
     struct IMessage_ :  public IMessage, public T {          
          std::condition_variable stateConditionVariable;
          std::mutex stateMutex; 

          virtual void OnStateChange(MessageState::type newState) { 
               currentState.streamState = newState;
               stateConditionVariable.notify_all();
          }
          virtual void OnStartLine(const rikitiki::string& sl) {
               SetStartline(sl);
          }
          virtual void OnHeader(const rikitiki::string& name, const  rikitiki::string& value) {
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
