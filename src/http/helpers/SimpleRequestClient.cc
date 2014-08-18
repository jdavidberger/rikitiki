#include <rikitiki/http/helpers/SimpleRequestClient.h>
#include <rikitiki/http/Response.h>
#include <rikitiki/rikitiki>
#include <sstream>
#include <mxcomp/utf.h>

namespace rikitiki {

     
  Socket::Socket(SocketListener& _listener) : listener(_listener) {}
     size_t Socket::Send(const std::string& buffer) {
          return this->Send(&buffer[0], buffer.size());
     }

     SimpleRequestClient::~SimpleRequestClient() {
          
     }
#pragma warning (disable: 4355)
  SimpleRequestClient::SimpleRequestClient(const wchar_t* _host, uint16_t port) : host(_host),
										  response(new IResponseMemory()), 
                                                                                  socket(CreateTCPIPSocket(*this, _host, port)) {
     }
#pragma warning (default: 4355)
     void SimpleRequestClient::MakeRequest(Request& request) {
          std::wstringstream req;
          
          req << request.Startline() << std::endl;
          req << L"Host: " << host << std::endl;
          for (auto it : request.Headers()) {
               req << it.first << ": " << it.second << std::endl;
          }
          req << std::endl;
          req << mxcomp::utf::convert( request.Body().str() );

          std::string utf8 = mxcomp::utf::convert (req.str());
          socket->Send(utf8.data(), utf8.size());
     }
     void SimpleRequestClient::OnClose() {
          promise.set_value(response);
     }
     bool SimpleRequestClient::OnData(const char* data, size_t length) {
          return response->OnData(data, length);          
     }
  
}
