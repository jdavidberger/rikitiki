#include <rikitiki\socket.h>
#ifdef _MSC_VER
#pragma warning (disable: 4365 4574 4263 4264 )
#define _WINSOCKAPI_
// http://social.msdn.microsoft.com/Forums/vstudio/en-US/96202d94-f108-483b-b007-6d3de6e303f4/ntddiwin7sp1-is-missing-from-80-sdkddkverh
#define NOMINMAX
#define NTDDI_WIN7SP1                       0x06010100
#include <windows.h>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#pragma warning (default: 4365 4574 4263 4264 )
#endif

namespace rikitiki {

Socket::Socket() : OnClose(0), OnData(0) {}
size_t Socket::Send(const std::string& buffer) {
	return this->Send(&buffer[0], buffer.size());
}

#ifdef _MSC_VER

	TCPIPSocket::~TCPIPSocket() {
		running = false;
		closesocket(_socket);
		pollingThread.join();
	}

	TCPIPSocket::TCPIPSocket(wchar_t * host, uint16_t port) : running(false) {

		WSADATA wsaData;
		int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != NO_ERROR) {
			throw std::exception("WSAStartup function failed with error: %d\n", iResult);
		}
		addrinfoW *result = NULL;
		GetAddrInfoW(host, nullptr, nullptr, (addrinfoW**)&result);
		while (result && result->ai_family != 2) {
			result = result->ai_next;
		}
		if (result == 0) {
			WSACleanup();
			throw std::exception("could not resolve address");
		}


		_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (_socket == INVALID_SOCKET) {
			WSACleanup();
			throw std::exception("socket function failed with error: %ld\n", WSAGetLastError());			
		}

		sockaddr_in clientService;
		clientService.sin_family = AF_INET;
		clientService.sin_addr = ((sockaddr_in*)result->ai_addr)->sin_addr;
		clientService.sin_port = htons(port);

		int cresult = connect(_socket, (SOCKADDR*)&clientService, sizeof(clientService));
		if (cresult == SOCKET_ERROR) {			
			closesocket(_socket);
			WSACleanup();
			throw std::exception("connect failed with error: %d\n", WSAGetLastError());
		}
		running = true;
		pollingThread = std::thread([=]() {
			char buffer[512];
			while (running) {				
				int readBytes = recv(_socket, buffer, sizeof(buffer), 0);
				if (readBytes <= 0) {
					running = false; 
					if (OnClose) (*OnClose)();
					return;
				}
				else {
					if (OnData) (*OnData)(buffer, (size_t)readBytes);
				}
			}
		});
	}
	
	size_t TCPIPSocket::Send(const char* buffer, size_t length) {
		return (size_t)send(_socket, buffer, (int)length, 0);
	}


}
#endif