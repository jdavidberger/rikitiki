#pragma once

#include <string>
#include <map>
#include <rikitiki/http/content_types.h>
#include <vector>
#include <sstream>
#include <mxcomp/useful_macros.h>
#include <mxcomp/cont_stringbuf.h>
#include <assert.h>
#include <rikitiki/http/Enums.h>
#include <rikitiki/http/parsing/BufferedReader.h>
#include <rikitiki/http/parsing/MessageParserState.h>
#include <rikitiki/http/Header.h>

#pragma warning (disable: 4265)

#include <condition_variable>

#pragma warning (default: 4265)

namespace rikitiki {

    struct OMessage {
    protected:
        http::parsing::MessageParserState streamState = http::parsing::MessageParserState(
                http::parsing::MessageState::START_LINE);

        Encoding::t encoding = Encoding::UNKNOWN;
        size_t contentSize = (size_t) -1;

        virtual void SetState(http::parsing::MessageState::type st);

        virtual void OnStateChange(http::parsing::MessageState::type);

        virtual void HeadersDone();

    public:
        virtual ~OMessage();

        virtual void Done();

        virtual size_t WritePayloadData(const char *, size_t) = 0;

        virtual void WriteHeader(const Header &) = 0;

        virtual OMessage &operator<<(rikitiki::ContentType::t t);

        virtual OMessage &operator<<(const rikitiki::Cookie &t);

        virtual OMessage &operator<<(const rikitiki::Header &t);

        virtual size_t WriteData(const char *, size_t) = 0;

        size_t WritePayloadData(const std::string &buff);
    };

    struct OMessageWriter : public virtual OMessage {
    protected:

        std::string buffer;

        virtual size_t WriteStartLine() = 0;

        using OMessage::WriteData;

        size_t WriteData(const wchar_t *, size_t);

        size_t WriteData(const std::wstring &);

        size_t WriteData(const std::string &);

    public:
        virtual void Done() OVERRIDE;

        virtual ~OMessageWriter() {};

        virtual void WriteHeader(const Header &) OVERRIDE;

        virtual size_t WritePayloadData(const char *, size_t) OVERRIDE;

        size_t WritePayloadData(const std::string &buff);
    };
    /*
    template <class T> auto operator <<(OMessage& out, const T& obj) -> decltype(std::declval<std::stringstream>() << obj, std::declval<OMessage&>())
    {
         std::stringstream ss;
         auto buff = mxcomp::attachContinousBuffer(ss);
         ss << obj;
         out.WriteData(buff->data(), buff->length());
         return out;
    }
    */
}
