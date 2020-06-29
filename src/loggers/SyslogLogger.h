//
// Created by satopja2 on 28.03.20.
//

#ifndef EIRSERVER_SYSLOG_LOGGER_H
#define EIRSERVER_SYSLOG_LOGGER_H

#include "Logger.h"

/**
 * Logger type class logging to syslog.
 */
class SyslogLogger: public Logger {
    public:
        /**
         * Sets logger verbosity (m_verbosity), opens syslog and logs start message.
         * @param[in] verbosity Verbosity of logger.
         */
        SyslogLogger(const string &verbosity);
        /**
         * Logs closing message and closes syslog.
         */
        ~SyslogLogger();
        /**
         * Gets current date and sets logged message type. Logs message to syslog.
         * @param[in] type Type of logged message.
         * @param[in] message Message text to be logged.
         * @see set_log_type()
         */
        virtual void log_message(const log_types &type, const string &message) noexcept override;
        /**
         * Logs minimal HTTP request and response code to syslog. Ignores verbose mode (never includes HTTP headers).
         * @param[in] request HTTP request data received from client.
         * @param[in] response HTTP response data sent by server.
         * @param[in] ip IP of client.
         */
        virtual void log_http(const string &request, const string &response, const string &ip) noexcept override;
    private:
        /** Member holding type of syslog message. */
        int m_type;
        /**
         * Sets syslog type of logged message.
         * @param[in] type Type of logged message.
         * @see log_types
         */
        virtual void set_log_type(const log_types &type) noexcept override;
};


#endif //EIRSERVER_SYSLOG_LOGGER_H
