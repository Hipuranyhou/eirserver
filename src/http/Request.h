//
// Created by satopja2 on 10.03.20.
//

#ifndef EIRSERVER_REQUEST_H
#define EIRSERVER_REQUEST_H

#include <string>
#include <arpa/inet.h>
#include <memory>
#include <map>

#include "../server/Config.h"
#include "../server/Cache.h"
#include "../loggers/Logger.h"
#include "../generators/Generator.h"
#include "../server/Path.h"
#include "Response.h"
#include "HttpConstants.h"

using namespace std;

/**
 * Class handling client HTTP requests.
 */
class Request {
    public:
        /**
         * Sets pointer to loaded configuration (m_config), pointer to active cache (m_cache), pointer to active
         * logger (m_logger), creates pointer to server HTTP response (m_response) and setups m_file with
         * path to root_dir from configuration.
         * @param[in] config Pointer to server configuration.
         * @param[in] cache Pointer to server cache.
         * @param[in] logger Pointer to server logger.
         */
        Request(shared_ptr<Config> config, shared_ptr<Cache> cache, shared_ptr<Logger> logger):
            m_response(make_unique<Response>()), m_config(config), m_cache(cache), m_logger(logger),
            m_file(m_config->find_setting_val("root_dir")) {}
        /**
         * Sets m_ip, m_request_data and parses HTTP request. \n
         * For bad request sets response to HttpConstants::CODE_BAD_REQUEST and returns. \n
         * For invalid HTTP protocol version sets response to HttpConstants::CODE_HTTP_VERSION and returns. \n
         * For unknown HTTP method sets response to HttpConstants::CODE_NOT_IMPLEMENTED and returns. \n
         * If requested path is equal to server shutdown path calls raise(SIGTERM). \n
         * Checks if file exists and for nonexistent file sets response to HttpConstants::CODE_NOT_FOUND and returns. \n
         * For valid request checks cache and for not modified file sets response to
         * HttpConstants::CODE_NOT_MODIFIED and returns. \n
         * Finally for modified or not cached file constructs response body and returns.\n
         * @param[in] request_data Complete data of client request.
         * @param[in] ip IP of client.
         * @see Response
         * @see HttpConstants
         * @see parse()
         * @see construct_body()
         * @see get_response()
         * @return String containing full HTTP response.
         */
        string handle(const string &request_data, const char ip[INET_ADDRSTRLEN]) noexcept;
        /**
         * Resets all members to their default state excluding m_config, m_cache and m_logger.
         */
        void reset() noexcept;
    private:
        /** Member holding HTTP response to current HTTP request. */
        unique_ptr<Response> m_response;
        /** Member holding pointer to server configuration. */
        shared_ptr<Config> m_config;
        /** Member holding pointer to server cache. */
        shared_ptr<Cache> m_cache;
        /** Member holding pointer to server logger. */
        shared_ptr<Logger> m_logger;
        /** Member holding complete data of client HTTP request. */
        string m_request_data;
        /** Member holding client IP address. */
        string m_ip;
        /** Member holding requested HTTP method. */
        HttpConstants::http_methods m_method;
        /** Member holding requested HTTP protocol version */
        string m_version;
        /** Struct holding information about requested file. */
        struct file {
            /**
             * Sets root_dir of requested file.
             * @param[in] root_dir Path to server root directory.
             */
            file(const string &root_dir): path(root_dir) {}
            /** Member holding requested path. */
            Path path;
            /** Member holding mime type of requested file. */
            string mime;
            /** Member holding etag value of requested file. */
            string etag;
        };
        /** Member holding information about requested file. */
        struct file m_file;
        /** Member holding HTTP response code. */
        string m_code;
        /**
         * Sets response method, code, all headers and calls construct() on m_response.
         * @see Response
         * @return String containing full HTTP response.
         */
        string get_response() noexcept;
        /**
         * Chooses which response body \ref Generator "generator" to use. ScriptGenerator for files with extension '.sh'.
         * RegularGenerator for regular files and directories containing file named 'index.html'.
         * DirectoryGenerator for directories.
         * @return Pointer to response body generator. Nullptr if no generator is chosen (should not happen).
         */
        unique_ptr<Generator> get_generator();
        /**
         * Gets pointer to response body \ref Generator "generator". If no \ref Generator "generator" is set
         * sets response to HttpConstants::CODE_INTERNAL_ERROR. Tries to set response body using
         * \ref Generator "generator". For text file without extension sets mime to 'text/plain' and tries to
         * add file to server cache.
         * @throw runtime_error If it is unable to check file or get its contents.
         * @see Generator
         * @see Cache
         * @see get_generator()
         */
        void construct_body() noexcept;
        /**
         * Tries to guess mime type of requested file based on its extension and store it in m_file.mime.
         * @param extension Extension of requested file.
         */
        void set_mime(const string &extension) noexcept;
        /**
         * Tries to find given header in m_request_data and sets its value to destination.
         * @param[in] header HTTP request header we search for
         * @param[out] destination Where should the header value be stored
         * @return true if header found and value extracted, false otherwise
         */
        bool extract_header(const string &header, string &destination) noexcept;
        /**
         * Extracts request method to m_method, path to m_file.path and HTTP version to m_version from
         * first line of request.
         * @note Because first line of request looks like "METHOD /path/to/resource HTTP_VERSION",
         * this function extracts all of them.
         */
        void parse_status_line () noexcept;
        /**
         * Sets m_method, m_file.path and m_version. Decodes requested URL to m_file.path
         * Sets m_file.mime and m_file.etag.
         * @see parse_status_line()
         * @see decode_url()
         * @see set_mime()
         * @see extract_header()
         */
        void parse() noexcept;
        /**
         * Decodes requested URL in percent encoding and stores its decoded value in m_file.path.
         * @param[in] url URL to be decoded.
         * @note Adjusted code from https://www.rosettacode.org/wiki/URL_decoding#C
         */
        void decode_url(const string &url) noexcept;
};

#endif //EIRSERVER_REQUEST_H
