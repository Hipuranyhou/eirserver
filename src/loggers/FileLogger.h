//
// Created by satopja2 on 28.03.20.
//

#ifndef EIRSERVER_FILE_LOGGER_H
#define EIRSERVER_FILE_LOGGER_H

#include <string>
#include <fstream>

#include "Logger.h"

/**
 * Logger type class logging to file.
 */
class FileLogger: public Logger {
    public:
        /**
         * Sets logger verbosity (m_verbosity), opens log file and logs start message.
         * @param[in] verbosity Verbosity of logger.
         * @param[in] log_file %Path to log file.
         * @throw runtime_error If log file cannot be opened.
         */
        FileLogger(const string &verbosity, const string &log_file);
        /**
         * Logs closing message and closes log file. If closing log file fails it makes note of that to syslog and cerr.
         */
        ~FileLogger();
        /**
         * Gets current date and sets logged message type. Logs message to log file. If logging to file fails
         * it makes note of that to syslog and cerr.
         * @param[in] type Type of logged message.
         * @param[in] message Message text to be logged.
         * @see set_log_type()
         * @see get_date()
         */
        virtual void log_message(const log_types &type, const string &message) noexcept override;
        /**
         * Logs minimal HTTP request and response code to log file. In verbose mode includes HTTP headers. If logging
         * to file fails it makes note of that to syslog and cerr.
         * @param[in] request HTTP request data received from client.
         * @param[in] response HTTP response data sent by server.
         * @param[in] ip IP of client.
         */
        virtual void log_http(const string &request, const string &response, const string &ip) noexcept override;
    private:
        /** Member holding log file output stream. */
        ofstream m_log_file;
};


#endif //EIRSERVER_FILE_LOGGER_H
