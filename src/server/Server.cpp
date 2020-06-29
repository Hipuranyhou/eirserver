//
// Created by satopja2 on 08.03.20.
//

#include <cstring>
#include <string>
#include <sys/unistd.h>
#include <sys/socket.h>
#include <csignal>
#include <stdexcept>

#include "Server.h"
#include "../loggers/ConsoleLogger.h"
#include "../loggers/SyslogLogger.h"
#include "../loggers/FileLogger.h"

// Initialize static members
map<string, string> Server::mimes = {
        {".aac", "audio/aac"},
        {".avi", "video/x-msvideo"},
        {".conf", "text/plain"},
        {".css", "text/css"},
        {".gif", "image/gif"},
        {".html", "text/html"},
        {".htm", "text/html"},
        {".ico", "image/vnd.microsoft.icon"},
        {".ini", "text/plain"},
        {".jpeg", "image/jpeg"},
        {".jpg", "image/jpeg"},
        {".js", "text/javascript"},
        {".json", "application/json"},
        {".mp3", "audio/mpeg"},
        {".otf", "font/otf"},
        {".pid", "text/plain"},
        {".png", "image/png"},
        {".sh", "text/html"},
        {".svg", "image/svg+xml"},
        {".ttf", "font/ttf"},
        {".txt", "text/plain"},
        {".woff", "font/woff"},
        {".woff2", "font/woff2"},
        {".xhtml", "application/xhtml+xml"},
        {".xml", "application/xml"}
};
int Server::catched_signal = 0;

Server::Server(const string &config) {
    string error_message, logger_type, logger_verbosity;
    memset(&m_server, 0, sizeof(m_server));
    memset(&m_client, 0, sizeof(m_client));

    // Initialize server configuration
    try {
        m_config = make_shared<Config>(config);
    } catch (const runtime_error& e) {
        error_message = "Config file error: " + string(e.what());
        throw runtime_error(error_message);
    }

    // Initialize server logger
    logger_verbosity = m_config->find_setting_val("verbosity");
    logger_type = m_config->find_setting_val("log_type");
    if (logger_type == "console"s)
        m_logger = make_shared<ConsoleLogger>(logger_verbosity);
    else if (logger_type == "syslog"s)
        m_logger = make_shared<SyslogLogger>(logger_verbosity);
    else {
        try {
            m_logger = make_shared<FileLogger>(logger_verbosity, m_config->find_setting_val("log_file"));
        } catch (const runtime_error& e) {
            error_message = "Logger error: " + string(e.what());
            throw runtime_error(error_message);
        }
    }

    // Initialize server cache
    m_cache = make_shared<Cache>(stoi(m_config->find_setting_val("cache_time")));

    // Prepare server socket struct
    string server_ip = m_config->find_setting_val("ip");
    m_server.addr.sin_family = AF_INET;
    if (inet_pton(AF_INET, server_ip.c_str(), &m_server.addr.sin_addr) == 0)
        throw runtime_error("Config file error: ip address is invalid");
    m_server.addr.sin_port = htons(stoi(m_config->find_setting_val("port")));

    register_signals();

    return;
}

Server::~Server() {
    close(m_client.fd);
    close(m_server.fd);
    return;
}

bool Server::start() noexcept {
    if (!setup())
        return false;
    string request;
    string response;
    char request_ip[INET_ADDRSTRLEN] = {0};
    m_request = make_unique<Request>(m_config, m_cache, m_logger);

    // Main control loop
    for (;;) {
        // Handle termination signals
        if (Server::catched_signal) {
            m_log_message = "Catched signal number: " + to_string(Server::catched_signal);
            m_logger->log_message(Logger::WARNING, m_log_message);
            return true;
        }

        // Accept client connection
        m_client.fd = accept(m_server.fd, (struct sockaddr*)&m_client.addr, (socklen_t*)&m_client.addr_len);
        if (m_client.fd < 0) {
            m_log_message = "Unable to accept connections (queue may be full)";
            m_logger->log_message(Logger::ERROR, m_log_message);
            m_request->reset();
            close(m_client.fd);
            continue;
        }

        // Get client IP address
        if (inet_ntop(AF_INET, &m_client.addr.sin_addr, request_ip, sizeof(request_ip)) == NULL)
            strncpy(request_ip, "Invalid IP", INET_ADDRSTRLEN);

        // Receive data from client
        switch (recv_all(request)) {
            case -1:
                m_log_message = "Unable to receive data from client -> " + string(request_ip);
                m_logger->log_message(Logger::ERROR, m_log_message);
                m_request->reset();
                close(m_client.fd);
                continue;
            case 0:
                m_log_message = "Client disconnected -> " + string(request_ip);
                m_logger->log_message(Logger::ERROR, m_log_message);
                m_request->reset();
                close(m_client.fd);
                continue;
            case 1:
                break;
        }

        // Get response to request
        response = m_request->handle(request, request_ip);

        // Handle termination signals
        if (Server::catched_signal) {
            m_log_message = "Shutdown requsted";
            m_logger->log_message(Logger::WARNING, m_log_message);
            return true;
        }

        // Send response to client
        if (!send_all(response)) {
            m_log_message = "Unable to send full response to client -> " + string(request_ip);
            m_logger->log_message(Logger::ERROR, m_log_message);
            m_request->reset();
            close(m_client.fd);
            continue;
        }

        m_request->reset();
        close(m_client.fd);
    }

    return true;
}

bool Server::setup() noexcept {
    // Get server socket file descriptor
    m_server.fd = socket(PF_INET, SOCK_STREAM, 0);
    if (m_server.fd < 0) {
        m_log_message = "Unable to get server socket file descriptor";
        m_logger->log_message(Logger::ERROR, m_log_message);
        return false;
    }

    // Enable reusing of socket, for example if the server crashes and starts again immediately
    int enable = 1;
    if (setsockopt(m_server.fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0) {
        m_log_message = "Unable to set server socket options";
        m_logger->log_message(Logger::ERROR, m_log_message);
        return false;
    }

    // Bind server socket to port
    if (bind(m_server.fd, (struct sockaddr*)&m_server.addr, sizeof(m_server.addr)) < 0) {
        m_log_message = "Unable to bind server socket to port " + m_config->find_setting_val("port");
        m_logger->log_message(Logger::ERROR, m_log_message);
        return false;
    }

    // Start listening on our port for incoming connections
    if (listen(m_server.fd, Server::active_incoming) < 0) {
        m_log_message = "Unable to start listening on port " + m_config->find_setting_val("port");
        m_logger->log_message(Logger::ERROR, m_log_message);
        return false;
    }

    return true;
}

void Server::register_signals() noexcept {
    signal(SIGTERM, Server::terminate);
    return;
}

void Server::terminate(int signum) noexcept {
    Server::catched_signal = signum;
    return;
}

int Server::recv_all(string &request) noexcept {
    int recv_val = 0;
    char buffer[Server::recv_buffer_size];
    request.clear();

    recv_val = recv(m_client.fd, buffer, Server::recv_buffer_size, 0);

    // Client disconnected or recv() error
    if (recv_val <= 0)
        return recv_val;

    request.append(buffer, recv_val);

    return 1;
}

bool Server::send_all(const string &response) noexcept {
    int bytes_sent = 0, bytes_left = response.length();
    const char* response_c_str = response.c_str();

    // Send all data
    while (bytes_left > 0) {
        bytes_sent = send(m_client.fd, response_c_str + bytes_sent, bytes_left, 0);
        if (bytes_sent == -1)
            return false;
        bytes_left -= bytes_sent;
        bytes_sent += bytes_sent;
    }

    return true;
}