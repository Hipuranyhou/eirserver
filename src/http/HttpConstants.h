//
// Created by satopja2 on 10.03.20.
//

#ifndef EIRSERVER_HTTP_CONSTANTS_H
#define EIRSERVER_HTTP_CONSTANTS_H

using namespace std;

/**
 * Helper class holding all HTTP related constants.
 */
class HttpConstants {
    public:
        /**
         * Enum holding all implemented HTTP request methods.
         * @note Implemented are only two mandatory methods GET and HEAD as seen in
         * https://tools.ietf.org/html/rfc2616#section-5.1.1
         */
        enum http_methods {
            METHOD_GET,
            METHOD_HEAD,
            METHOD_UNKNOWN,
            METHOD_ERROR
        };

        // 2xx implemented codes
        /** HTTP response code "200 Ok" */
        static constexpr const char* CODE_OK = "200 Ok";

        // 3xx implemented codes
        /** HTTP response code "304 Not Modified" */
        static constexpr const char* CODE_NOT_MODIFIED = "304 Not Modified";

        // 4xx implemented codes
        /** HTTP response code "400 Bad Request" */
        static constexpr const char* CODE_BAD_REQUEST = "400 Bad Request";
        /** HTTP response code "404 Not Found" */
        static constexpr const char* CODE_NOT_FOUND = "404 Not Found";

        // 5xx implemented codes
        /** HTTP response code "500 Internal Server Error" */
        static constexpr const char* CODE_INTERNAL_ERROR = "500 Internal Server Error";
        /** HTTP response code "501 Not Implemented" */
        static constexpr const char* CODE_NOT_IMPLEMENTED = "501 Not Implemented";
        /** HTTP response code "505 HTTP Version Not Supported" */
        static constexpr const char* CODE_HTTP_VERSION = "505 HTTP Version Not Supported";
};

#endif //EIRSERVER_HTTP_CONSTANTS_H
