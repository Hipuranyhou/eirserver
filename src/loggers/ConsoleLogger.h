//
// Created by satopja2 on 28.03.20.
//

#ifndef EIRSERVER_CONSOLE_LOGGER_H
#define EIRSERVER_CONSOLE_LOGGER_H

#include "Logger.h"

/**
 * Logger type class logging to console.
 */
class ConsoleLogger: public Logger {
    public:
        /**
         * Sets logger verbosity (m_verbosity) and logs start message to cout.
         * @param[in] verbosity Verbosity of logger.
         */
        ConsoleLogger(const string &verbosity);
        /**
         * Logs closing message to cout.
         */
        ~ConsoleLogger();
        /**
         * Gets current date and sets logged message type. Logs message to cout or cerr (for type ERROR).
         * @param[in] type Type of logged message.
         * @param[in] message Message text to be logged.
         * @see set_log_type()
         * @see get_date()
         */
        virtual void log_message(const log_types &type, const string &message) noexcept override;
        /**
         * Logs minimal HTTP request and response code to cout. In verbose mode includes HTTP headers.
         * @param[in] request HTTP request data received from client.
         * @param[in] response HTTP response data sent by server.
         * @param[in] ip IP of client.
         */
        virtual void log_http(const string &request, const string &response, const string &ip) noexcept override;
};

#endif //EIRSERVER_CONSOLE_LOGGER_H
