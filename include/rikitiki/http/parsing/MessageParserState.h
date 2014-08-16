#pragma once

#include <rikitiki/http/Enums.h>

namespace rikitiki {
     namespace http {
          namespace parsing {
               namespace BodyType{
                    enum type {
                         UNKNOWN,
                         // No body
                         NONE,
                         // Not specified in any discernable way in the given headers
                         BUFFERING,
                         // Content-Length was set
                         KNOWN_SIZE,
                         // Transfer encoding was chunked
                         CHUNKED
                    };
                    type From(size_t length, Encoding::t enc);
               }
               namespace MessageState {
                    enum type {
                         UNKNOWN,
                         START_LINE,
                         HEADERS,                         
                         BODY,
                         FINISHED
                    };
               }
  
               struct MessageParserState {
                    BodyType::type bodyType = BodyType::UNKNOWN;
                    MessageState::type streamState = MessageState::START_LINE;
                    explicit MessageParserState(MessageState::type streamState = MessageState::START_LINE,
                         BodyType::type streamType = BodyType::UNKNOWN);
               };

          }
     }
}
