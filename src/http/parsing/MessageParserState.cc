#include <rikitiki\http\parsing\MessageParserState.h>

using namespace rikitiki::http::parsing;
using namespace rikitiki;

BodyType::type BodyType::From(size_t contentSize, Encoding::t encoding) {
     if (contentSize != -1) {
          return KNOWN_SIZE;
     }
     else if (encoding == Encoding::chunked) {
          return CHUNKED;
     }
     else {
          return NONE;
     }
}

rikitiki::http::parsing::MessageParserState::MessageParserState(MessageState::type _streamState, BodyType::type streamType) :
streamState(_streamState), bodyType(streamType)
{

}
