#pragma once
#include <stdint.h>

#include <mxcomp\useful_macros.h>

#pragma warning(disable:4265)
#include <thread>
#pragma warning(default:4265)
typedef _W64 unsigned int UINT_PTR, *PUINT_PTR;
typedef UINT_PTR        SOCKET;

namespace rikitiki {
	struct Socket {
		Socket();
		virtual size_t Send(const char*, size_t length) = 0;
		size_t Send(const std::string&);
		typedef void (*OnCloseFunction)();
		typedef void(*OnDataFunction)(const char*, size_t length);

		OnCloseFunction OnClose; 
		OnDataFunction OnData;
	};

	class TCPIPSocket : public Socket {
#ifdef _MSC_VER
		SOCKET _socket; 
		std::thread pollingThread;
		void poll();
		bool running; 
#endif
	public:
		TCPIPSocket(wchar_t* host, uint16_t port = 80);
		virtual ~TCPIPSocket();
		virtual size_t Send(const char*, size_t length) OVERRIDE;
	};


}