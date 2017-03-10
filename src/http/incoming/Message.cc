#include <rikitiki/exception.h>
#include <rikitiki/http/parsing/Utils.h>
#include <rikitiki/http/Header.h>
#include <rikitiki/http/incoming/Message.h>

#include <mxcomp/log.h>
#include <mxcomp/utf.h>

namespace rikitiki {
     void IMessage::OnStartLine(const rikitiki::string& startline) {
          this->SetStartline(startline);
     }
     void IMessage::OnHeader(const rikitiki::string& n, const  rikitiki::string& v) {
          *this << Header(n, v);
     }
     void IMessage::OnBodyData(const char* data, size_t size) {
          Body().write(data, (std::streamsize)size);
     }
     void IMessage::UpdateBufferState() {
          if (currentState.streamState == MessageState::BODY) {
               currentState.bodyType = BodyType::From(this->ContentLength(), this->TransferEncoding());
               if (currentState.bodyType == BodyType::KNOWN_SIZE) {
                    bufferMode = BufferModeT::LENGTH;
                    expectedSize = ContentLength();
               }
               if (currentState.bodyType == BodyType::CHUNKED)
                    bufferMode = BufferModeT::NEWLINE; // To read in the next chunk size
               if (currentState.bodyType == BodyType::NONE)
                    currentState.streamState = MessageState::FINISHED;
          }
     }

     bool IMessage::OnBufferedData(const char* data, std::size_t length) {
          std::string buffer;

	  LOG(IMessage, Debug) << "Buffered IMessage: " << currentState.streamState << ", " << currentState.bodyType << ": " << data << std::endl;

          const char* end = &data[length];
          switch (currentState.streamState) {
          case MessageState::START_LINE: {
               OnStartLine(data);
               SetState(MessageState::HEADERS);
               return false;
          }
          case MessageState::HEADERS:
               if (data[0] == '\r' || data[0] == '\n') {
                    data += 2;
                    currentState.bodyType = BodyType::From(ContentLength(), TransferEncoding());
                    SetState(ContentLength() == 0 ? MessageState::FINISHED : MessageState::BODY);                    
                    bufferMode = TransferEncoding() == Encoding::chunked ? NEWLINE : NONE;
                    expectedSize = ContentLength();
               }
               else {                    
                    std::string buffer2;
                    data = readHeaderName(data, end, buffer);
                    data = readHeaderValue(data, end, buffer2);                    
                    OnHeader(rikitiki::to_rt_string(&buffer[0]),
                             rikitiki::to_rt_string(&buffer2[0]));
                    data += 2; // Burn \r\n                    
               }
               return currentState.streamState == MessageState::FINISHED;
          case MessageState::BODY: {
               switch (currentState.bodyType) {
               case BodyType::CHUNKED: {
                    if (expectedSize == (size_t)-1) {
                         expectedSize = (size_t)strtol(data, (char**)&data, 16) + 2;
                         bufferMode = LENGTH;
                    }
                    else {
                         auto relevantSize = (std::streamsize)(expectedSize - 2);
                         OnChunkedData(data, (size_t)relevantSize); 
                         if (expectedSize - 2 == 0)
                              currentState.streamState = MessageState::FINISHED;
                         bufferMode = NEWLINE;
                    }
                    break;
               }
               case BodyType::KNOWN_SIZE:
                    if (data != end) {
                         bufferMode = NONE;
                         OnBodyData(data, (size_t)(end - data));
                         if ((size_t)(end - data) <= expectedSize) {
                              expectedSize -= (size_t)(end - data);
                         }
                         else {
                              expectedSize = 0; 
                              LOG(Message, Warning) << "Data in message was longer than expected." << std::endl;
                         }
                    }

                    if (expectedSize == 0)
		      SetState(MessageState::FINISHED);
                    break;
               case BodyType::BUFFERING:
               case BodyType::UNKNOWN:
                    OnBodyData(data, (size_t)(end - data));
                    break;
               }
               break;
          }
          case MessageState::FINISHED:
               throw new rikitiki::exception("Unexpected input");
          }

	  if(currentState.streamState == MessageState::FINISHED) {
	    LOG(IMessage, Debug) << "Message finished" << std::endl;
	  }

          return currentState.streamState == MessageState::FINISHED;
     }
     void IMessage::SetState(MessageState::type newState) { 
       OnStateChange(newState);
       currentState.streamState = newState; 
     }
     bool IMessage::OnData(const char* data, size_t len) {
          LOG(Message, Debug) << (void*)this << " incoming: " << std::string(data, len) << std::endl;
          return BufferedReader::OnData(data, len);
     }

     void MessageListener::OnChunkedData(const char* d, size_t len) {
          OnBodyData(d, len);
     }
}
