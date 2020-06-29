//
// Created by satopja2 on 28.03.20.
//

#ifndef EIRSERVER_LOGGER_H
#define EIRSERVER_LOGGER_H

#include <string>

using namespace std;

/**
 * Abstract class for logger types.
 */
class Logger {
    public:
        /**
         * Default destructor.
         */
        virtual ~Logger() = default;
        /**
         * Enum holding types of log messages.
         */
        enum log_types {
            ERROR,
            WARNING,
            INFO
        };
        /**
         * Pure virtual function. Logs message.
         * @param[in] type Type of logged message.
         * @param[in] message Message text to be logged.
         */
        virtual void log_message(const log_types &type, const string &message) noexcept = 0;
        /**
         * Pure virtual function. Logs HTTP request and response.
         * @param[in] request HTTP request data received from client.
         * @param[in] response HTTP response data sent by server.
         * @param[in] ip IP of client.
         */
        virtual void log_http(const string &request, const string &response, const string &ip) noexcept = 0;
    protected:
        /** Member holding verbosity of logger. */
        string m_verbosity;
        /** Member holding body of logged message. */
        string m_body;
        /**
         * Gets current date and time.
         * @return String containing current date and time in format "%d.%m.%Y %T".
         */
        string get_date() noexcept;
        /**
         * Appends type of logged message to m_body.
         * @param[in] type Type of logged message.
         */
        virtual void set_log_type(const log_types &type) noexcept;
        /**
         * Extracts request status line (example: "GET /favicon.ico HTTP/1.1").
         * @param[in] request Data of HTTP request.
         * @return String containing minimal request in format "HTTP_METHOD REQUEST_PATH".
         */
        string extract_request_status(const string &request) noexcept;
        /**
         * Extracts all headers, line by line, from HTTP request and appends them to m_body.
         * @param[in] request Data of HTTP request.
         * @param[in] format_width Width of body for verbose formatting (log_type + date).
         */
        void append_request_headers(const string &request, const int &format_width) noexcept;
        /**
         * Extracts HTTP code from response data (example: "304 Not Modified").
         * @param[in] response Data of HTTP response.
         * @return String containing HTTP code of server response.
         */
        string extract_response_code(const string &response) noexcept;
        /**
         * Extracts all headers, line by line, from HTTP response and appends them to m_body.
         * @param[in] response Data of HTTP response.
         * @param[in] format_width Width of body for verbose formatting (log_type + date).
         */
        void append_response_headers(const string &response, const int &format_width) noexcept;
        /**
         * Gets current date, appends log header to m_body and in \ref Verbosity "verbose" mode
         * appends all headers to m_body.
         * @param[in] request %Request data received from client.
         * @param[in] response %Response data sent by server.
         * @param[in] ip IP of client.
         * @see get_date()
         * @see extract_request_status()
         * @see append_request_headers()
         * @see extract_response_code()
         * @see append_response_headers()
         */
        void construct_body(const string &request, const string &response, const string &ip) noexcept;
};


#endif //EIRSERVER_LOGGER_H
