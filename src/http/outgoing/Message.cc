#include <mxcomp/log.h>
#include <mxcomp/utf.h>

#include <rikitiki/exception.h>
#include <rikitiki/http/Header.h>
#include <rikitiki/http/outgoing/Message.h>
#include <rikitiki/http/parsing/MessageParserState.h>
#include <rikitiki/http/parsing/InvalidStateException.h>

#include <locale>
#include <algorithm>
#include <assert.h>

namespace rikitiki {
    using namespace http::parsing;

    OMessage &OMessage::operator<<(rikitiki::ContentType::t t) {
        this->WriteHeader(Header(RT_STRING_LITERAL"Content-Type", ContentType::ToString(t)));
        return *this;
    }

    OMessage &OMessage::operator<<(const rikitiki::Cookie &t) {
        *this << Header(RT_STRING_LITERAL"Set-Cookie", t.first + RT_STRING_LITERAL"=" + t.second);
        return *this;
    }

    OMessage &OMessage::operator<<(const rikitiki::Header &t) {
        this->WriteHeader(t);
        return *this;
    }

    void OMessage::SetState(MessageState::type st) {
        OnStateChange(st);
        streamState.streamState = st;
    }

    void OMessage::WriteHeader(const rikitiki::Header &) {
        switch (streamState.streamState) {
            case MessageState::START_LINE:
                SetState(MessageState::HEADERS);
            case MessageState::HEADERS:
                return;
            case MessageState::BODY:
            case MessageState::FINISHED:
            default:
                throw InvalidStateException("Could not write header");
        }
    }

    size_t OMessage::WritePayloadData(const char *, size_t size) {
        switch (streamState.streamState) {
            case MessageState::START_LINE:
                SetState(MessageState::HEADERS);
            case MessageState::HEADERS:
                streamState.bodyType = BodyType::From(contentSize, encoding);
                if (streamState.bodyType == BodyType::NONE)
                    streamState.bodyType = BodyType::BUFFERING;
                SetState(MessageState::BODY);
            case MessageState::BODY:
                return size;
            case MessageState::FINISHED:
            default:
                throw InvalidStateException("Could not write payload");
        }
    }

    size_t OMessageWriter::WritePayloadData(const char *buffer, size_t size) {
        auto oldStreamState = streamState.streamState;
        if (streamState.streamState == MessageState::START_LINE) {
            WriteStartLine();
        }

        OMessage::WritePayloadData(buffer, size);

        if (oldStreamState <= MessageState::HEADERS && streamState.bodyType != BodyType::BUFFERING)
            WriteData("\r\n");

        switch (streamState.bodyType) {
            case BodyType::BUFFERING:
                this->buffer.append(buffer, size);
                return size;
            case BodyType::CHUNKED: {
                std::stringstream ss;
                ss << std::hex << size << "\r\n";
                return WriteData(ss.str()) +
                       WriteData(buffer, size) +
                       WriteData("\r\n");
            }
            case BodyType::KNOWN_SIZE:
                return WriteData(buffer, size);
            case BodyType::UNKNOWN:
                throw InvalidStateException("Should not get here.");
        }
        throw InvalidStateException("Should not get here.");
    }

    size_t OMessage::WritePayloadData(const std::string &buff) {
        return WritePayloadData(buff.data(), buff.length());
    }

    OMessage::~OMessage() {}

    void OMessage::Done() {}

    void OMessage::OnStateChange(http::parsing::MessageState::type) {}

    void OMessage::HeadersDone() {}


    void OMessageWriter::Done() {
        switch (streamState.streamState) {
            case MessageState::START_LINE:
                WriteStartLine();
                SetState(MessageState::HEADERS);
            case MessageState::HEADERS:
                if (contentSize == -1 && encoding != Encoding::chunked) {
                    WriteHeader(Header(RT_STRING_LITERAL"Content-Length", RT_STRING_LITERAL"0"));
                    WriteData("\r\n");
                    return;
                } else if (encoding == Encoding::chunked) {
                    WriteData("\r\n0\r\n\r\n");
                    return;
                }
                WriteData("\r\n");
                if (contentSize != 0) {
                    throw rikitiki::exception("Connection was abandoned prematurely");
                }
            case MessageState::BODY:
                switch (streamState.bodyType) {
                    case BodyType::CHUNKED:
                        WriteData("0\r\n\r\n");
                        break;
                    case BodyType::BUFFERING: {
                        rikitiki::stringstream ss;
                        ss << buffer.size();
                        streamState.streamState = MessageState::HEADERS;
                        WriteHeader(Header(RT_STRING_LITERAL"Content-Length", ss.str()));
                        WriteData("\r\n");
                        WriteData(buffer);
                        break;
                    }
                    case BodyType::KNOWN_SIZE:
                    case BodyType::UNKNOWN:
                        throw InvalidStateException("");
                }
                break;
            case MessageState::FINISHED:
                return;
        }
    }

    void OMessageWriter::WriteHeader(const Header &header) {
        switch (streamState.streamState) {
            case MessageState::START_LINE:
                WriteStartLine();
                SetState(MessageState::HEADERS);
            case MessageState::HEADERS: {
                WriteData(header.first);
                WriteData(": ");
                WriteData(header.second);
                WriteData("\r\n");
                return;
            }
            default:
                throw InvalidStateException("Could not write header");
        }
    }

    size_t OMessageWriter::WriteData(const wchar_t *buffer, size_t size) {
        auto strBuffer = mxcomp::utf::convert(buffer, &buffer[size]);
        return WriteData(strBuffer.data(), strBuffer.length());
    }

    size_t OMessageWriter::WriteData(const std::wstring &str) {
        return WriteData(str.data(), str.length());
    }

    size_t OMessageWriter::WriteData(const std::string &str) {
        return WriteData(str.data(), str.length());
    }

}
