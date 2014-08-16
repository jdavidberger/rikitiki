#include <rikitiki/http/helpers/SimpleRequestClient.h>

#include <mxcomp/utf.h>
#include <mxcomp/log.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/tcp.h>

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/thread.h>

namespace rikitiki {
  struct LinuxTCPIPSocket : public TCPIPSocket {
    std::thread eventthread;
    struct bufferevent *bev;
    int8_t connect_status = 0; 
    
    LinuxTCPIPSocket(SocketListener& l, const wchar_t* host, uint16_t port);
    virtual ~LinuxTCPIPSocket();
    virtual size_t Send(const char*, size_t length) OVERRIDE;
    virtual void OnClose() OVERRIDE;
    void Cleanup();
  };
    
  TCPIPSocket* CreateTCPIPSocket(SocketListener& l, const wchar_t* host, uint16_t port) {
    return new LinuxTCPIPSocket(l, host, port);
  }
  
  void LinuxTCPIPSocket::Cleanup() {
    bufferevent_flush(bev, EV_WRITE, BEV_FINISHED);
    bufferevent_disable(bev, EV_READ|EV_WRITE);
    bufferevent_free(bev);
    bev = 0;
  }
  LinuxTCPIPSocket::~LinuxTCPIPSocket() {
    Cleanup();
  }

  size_t LinuxTCPIPSocket::Send(const char* buffer, size_t length) {
    LOG(SimpleRequestClient, Debug) << "Sending: " << buffer << std::endl;
    if(bufferevent_write(bev, buffer, length) == -1)
      return -1;
    return length;
  }

  void LinuxTCPIPSocket::OnClose()  {    
    Socket::OnClose();
    Cleanup();
    LOG(SimpleRequestClient, Debug)  << " Close!" << std::endl;
  }


  static void writecb(struct bufferevent *bev, void *ctx) {
    LOG(SimpleRequestClient, Debug) << "Flushing " << bev << std::endl;
  }

  static void readcb(struct bufferevent *bev, void *ctx)
  {
    int n;
    struct evbuffer_iovec v;
    struct evbuffer *buf = bufferevent_get_input(bev);
    LinuxTCPIPSocket* socket = (LinuxTCPIPSocket*)ctx;
    do {
      n = evbuffer_peek(buf, -1, NULL, &v, 1);
      if(n) {
   	bool done = socket->OnData((const char*)v.iov_base, v.iov_len); 
	LOG(SimpleRequestClient, Debug) << "Reading " << (const char*)v.iov_base << std::endl;
	evbuffer_drain(buf, v.iov_len); 

	if(done) {
	  socket->OnClose();	  
	}
      }
    } while(n);
       
  }

  static void eventcb(struct bufferevent *bev, short events, void *ctx)
  {
    LinuxTCPIPSocket* socket = (LinuxTCPIPSocket*)ctx;
    if (events & BEV_EVENT_CONNECTED) {
      evutil_socket_t fd = bufferevent_getfd(bev);
      int one = 1;
      setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &one, sizeof one);
      LOG(SimpleRequestClient, Debug) << "Connected" << std::endl;
      socket->connect_status = 1;
    } else if (events & BEV_EVENT_ERROR) {
      LOG(SimpleRequestClient, Debug) << "Connection error" << std::endl;
      socket->connect_status = -1;
    } else {
      LOG(SimpleRequestClient, Debug) << "Other event: " << events << std::endl;
    }
  }

  
  static uint32_t host_to_ip(const std::string& host) {
    struct hostent *he;
    struct in_addr **addr_list;
    int i;
         
    if ( (he = gethostbyname( host.data() ) ) == NULL) {
      return 0;
    }    

    addr_list = (struct in_addr **) he->h_addr_list;
     
    for(i = 0; addr_list[i] != NULL; i++) {
      return (addr_list[i]->s_addr);
    }
    return 0; 
  };

  struct threaded_event_loop {
    std::thread pump_thread;
    struct event_base* base = 0;
    bool running = false;
  };

  static void run_events(threaded_event_loop* loop) {
    LOG(SimpleRequestClient, Debug) << "Starting " << loop->base << std::endl;
    event_base_loop(loop->base,0);
    LOG(SimpleRequestClient, Debug) << "No more events, killing " << loop->base << std::endl;
    auto b = loop->base;
    loop->base = 0;
    event_base_free(b);
    loop->running = false; 
  }

  static threaded_event_loop * get_base() {
    static threaded_event_loop loop;
    if(loop.base == 0) {
      loop.base = event_base_new();
    }
    return &loop;
  }

  static void kick_events() {
    auto& loop = *get_base();
    if(loop.running == false) {
      if(loop.pump_thread.joinable())
	loop.pump_thread.join();
      loop.pump_thread = std::thread(run_events, &loop);    
    }
  }

  LinuxTCPIPSocket::LinuxTCPIPSocket(SocketListener& l,const wchar_t * host, uint16_t port) : TCPIPSocket(l) {
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = host_to_ip( mxcomp::utf::convert(host) );
    sin.sin_port = htons( port );
   
    evthread_use_pthreads();    
    
    auto base = get_base()->base;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (connect(sockfd, (struct sockaddr *)&sin, sizeof(sin)) != 0) {
      LOG(SimpleRequestClient, Debug) << "Couldn't connect to " << std::hex << ntohl(sin.sin_addr.s_addr) << ":" << std::dec << port << std::endl;      
      return;
    }
    LOG(SimpleRequestClient, Debug) << "Connected " << std::hex << ntohl(sin.sin_addr.s_addr) << ":" << std::dec << port << std::endl;      
    bev = bufferevent_socket_new(base, sockfd, BEV_OPT_CLOSE_ON_FREE);
    
    bufferevent_setcb(bev, readcb, writecb, eventcb, this);
    bufferevent_enable(bev, EV_READ|EV_WRITE);    

    evutil_make_socket_nonblocking(sockfd);
    kick_events();
  }
}
