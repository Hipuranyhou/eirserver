//
// Created by satopja2 on 10.03.20.
//

#ifndef EIRSERVER_RESPONSE_H
#define EIRSERVER_RESPONSE_H

#include <string>
#include <map>

#include "HttpConstants.h"

using namespace std;

/**
 * Class constructing HTTP response to HTTP request
 */
class Response {
    public:
        /**
         * Sets value of given HTTP response header (in m_headers).
         * @param[in] header HTTP response header name.
         * @param[in] value Value of HTTP response header.
         */
        void set_header(const string &header, const string &value) noexcept;
        /**
         * Sets method of HTTP response (m_method).
         * @param[in] method Response method.
         */
        void set_method(const HttpConstants::http_methods &method) noexcept;
        /**
         * Sets code of HTTP response (m_code).
         * @param[in] code Response code.
         */
        void set_code(const string &code) noexcept;
        /**
         * Sets body of HTTP response (m_body).
         * @param[in] data Data of response body.
         */
        void set_body(const string &data) noexcept;
        /**
         * Constructs string containing full HTTP response by appending all headers and body.
         * @return String containing full HTTP response.
         */
        string construct() noexcept;
        /**
         * Clears all members.
         */
        void reset() noexcept;
    private:
        /** Member map holding all HTTP response headers. */
        map<string, string> m_headers;
        /** Member holding HTTP response method. */
        int m_method;
        /** Member holding HTTP response code. */
        string m_code;
        /** Member holding HTTP response body. */
        string m_body;
        /**
         * Gets current date and time.
         * @return String containing current date and time in format "%a, %d %b %Y %T GMT".
         */
        string get_date() noexcept;
};


#endif //EIRSERVER_RESPONSE_H
