#pragma once
#include <stdint.h>

#include <mxcomp\useful_macros.h>
#include <memory>
#pragma warning(disable:4265 4355)
#include <thread>
#include <future>
#pragma warning(default:4265 4355)
#include <sstream>
#include <rikitiki\Response.h>

//typedef _W64 unsigned int UINT_PTR, *PUINT_PTR;
typedef unsigned long long        SOCKET;


namespace rikitiki {
     struct SocketListener {
          virtual ~SocketListener() {};
          virtual void OnClose() = 0;
          virtual bool OnData(const char*, size_t length) = 0;
     };

	struct Socket {
             virtual ~Socket() {};
             std::vector<SocketListener*> listeners; 
	       Socket();
	       virtual size_t Send(const char*, size_t length) = 0;
	       size_t Send(const std::string&);	       
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

        class Response;
        class IRequest; 
        class SimpleRequestClient : public SocketListener{
             TCPIPSocket socket;              
             std::shared_ptr<Response> response;
             std::promise<std::shared_ptr<Response>> promise;
             const wchar_t* host;
             std::stringstream localBuffer; 

             ResponseBuilder builder; 
        public:
             
             ~SimpleRequestClient();

             SimpleRequestClient(wchar_t* host, uint16_t port = 80);
             void MakeRequest(IRequest& request);
             virtual void OnClose();
             virtual bool OnData(const char*, size_t length);
             std::future<std::shared_ptr<Response>> future() {
                  return promise.get_future();
             }
        };
}