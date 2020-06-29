//
// Created by satopja2 on 08.03.20.
//

#ifndef EIRSERVER_SERVER_H
#define EIRSERVER_SERVER_H

#include <netinet/in.h>
#include <arpa/inet.h>
#include <memory>
#include <map>

#include "../http/Request.h"
#include "../loggers/Logger.h"
#include "Config.h"
#include "Cache.h"

using namespace std;

/**
 * Main server class.
 */
class Server {
    public:
        /**
         * Initializes server configuration (m_config), logger based on configuration (m_logger) and cache (m_cache).
         * Registers signal handlers.
         * @param[in] config %Path to config file which should eirserver use.
         * @throw runtime_error If config file contains errors or logger cannot be initialized.
         * @see Config
         * @see Logger
         * @see Cache
         * @see register_signals()
         */
        Server(const string &config);
        /**
         * Closes client and server sockets.
         */
        ~Server();
        /** Static map of all known mime types. */
        static map<string, string> mimes;
        /**
         * Setups server socket, m_request and starts main control loop.
         *
         * Calls setup() which prepares the server socket. Starts main control loop which
         * receives HTTP requests from clients, then calls HTTP request handler on them
         * and sends full HTTP responses back to clients. If registered termination signal is catched
         * it shuts the server down.
         *
         * It should really only return if we are shutting the server down.
         * @return true if registered signal is catched, false if setup() failed
         * @see Request
         * @see Response
         * @see setup()
         * @see recv_all()
         * @see send_all()
         */
        bool start() noexcept;
    private:
        /** Static member used for catching signals in main control loop. */
        static int catched_signal;
        /** Static member holding number of active connections which should listen() use. */
        static const int active_incoming = 10;
        /** Static member holding size of buffer for recv(). */
        static const int recv_buffer_size = 4096;
        /** Member holding pointer to current handled request. */
        unique_ptr<Request> m_request;
        /** Member holding pointer to loaded configuration. */
        shared_ptr<Config> m_config;
        /** Member holding pointer to active cache. */
        shared_ptr<Cache> m_cache;
        /** Member holding pointer to active logger. */
        shared_ptr<Logger> m_logger;
        /** Member holding current logged message. */
        string m_log_message;
        /**
         * Struct storing information about socket used by server.
         */
        struct sock {
            /** Member holding socket file descriptor. */
            int fd;
            /** Member holding network address information. */
            struct sockaddr_in addr;
            /** Member holding size of network address information. */
            size_t addr_len;
        };
        /** Member storing socket information about server. */
        struct sock m_server;
        /** Member storing socket information about client. */
        struct sock m_client;
        /**
         * Gets server socket file descriptor, enables its reusing, binds and starts listening on it.
         * @return Boolean if preparing the server socket was successful.
         */
        bool setup() noexcept;
        /**
         * Registers signal handlers for all implemented signals.
         * @note Currently implemented is only SIGTERM used for turning off the server
         * with \ref Shutdown "shutdown address" configured in config file.
         */
        void register_signals() noexcept;
        /**
         * Sets catched signal number (signum) to static member catched_signal for termination in main control loop.
         * @param[in] signum Catched signal number.
         * @see catched_signal
         */
        static void terminate(int signum) noexcept;
        /**
         * Receives data from client socket (m_client.fd).
         * @param[out] request Where should be the received data stored.
         * @return -1 if recv() encountered error, 0 if client disconnected, 1 if recv() was successful
         */
        int recv_all(string &request) noexcept;
        /**
         * Sends all HTTP response data through client socket (m_client.fd) to client.
         * @param[in] response Data to be sent.
         * @return true if sending all data was successful, false if send() encountered error
         * @note Because send() sometimes does not send all the data, we need to do this in a loop
         * and make sure that everything was truly sent.
         */
        bool send_all(const string &response) noexcept;
};

#endif //EIRSERVER_SERVER_H
