#pragma once

#include <string>

namespace rikitiki {
     namespace http {
          namespace parsing {
               class BufferedReader {
               protected:
                    size_t expectedSize = (size_t)-1;
                    enum BufferModeT {
                         LENGTH,
                         NEWLINE,
                         NONE
                    };
                    BufferModeT bufferMode;
                    std::string buffer;

                    virtual bool OnBufferedData(const char*, std::size_t length) = 0;
               public:
                    bool OnData(const char*, std::size_t length);
                    BufferedReader();
                    virtual ~BufferedReader(){}
               };
          }
     }
}