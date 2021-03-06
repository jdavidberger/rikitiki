/* Copyright (C) 2012-2013 Justin Berger
   The full license is available in the LICENSE file at the root of this project and is also available at http://opensource.org/licenses/MIT. */
#pragma once

#include <rikitiki/server.h>
#include <rikitiki/websocket/websocket>

extern "C" {
struct mg_context;
struct mg_connection;
struct mg_serve_http_opts;
}

namespace rikitiki {

    namespace mongoose {
        /**
           Mongoose wrapper. Contains base server functionality, plus also exposes Start/Stop methods.
           */
        class MongooseServer : public rikitiki::Server
#ifdef RT_USE_WEBSOCKET
                , public rikitiki::websocket::Server
#endif
        {
            mg_context *ctx;
            uint16_t port;
            std::thread runThread;

#ifdef RT_USE_WEBSOCKET
        private:
            static int _wsHandler(const struct mg_connection *conn);

            static void _wsReady(struct mg_connection *conn);

            static int _wsReceive(struct mg_connection *conn, int bits, char *data, size_t length);

        protected:
            virtual void Close(websocket::WebsocketContext *) OVERRIDE;

            virtual websocket::WebsocketProcess *HandleWs(websocket::ConnectionHandle) OVERRIDE;

        public:
            using rikitiki::websocket::Server::Register;
#endif
        public:
            using rikitiki::Server::Register;
            struct std::unique_ptr<mg_serve_http_opts> s_http_server_opts;

            virtual std::future<std::shared_ptr<Response>> ProcessRequest(Request &);

            std::string DocumentRoot;

            uint16_t Port();

            MongooseServer(uint16_t _port);

            ~MongooseServer();

            /** Blocking call to start the mongoose server. Sets up an interrupt handler to stop the server with siginterrupt.
                If you have more than one server running, SIGINT shuts them all down.
                */
            void Run();

            /** Non-blocking call to start the mongoose server.*/
            void Start();

            /** Wait for a started server to stop*/
            void WaitForStop();

            /** Stop a running server */
            void Stop();
        };
    }
}
