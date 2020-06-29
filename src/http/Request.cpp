//
// Created by satopja2 on 10.03.20.
//

#include <sstream>
#include <iterator>
#include <csignal>
#include <stdexcept>

#include "Request.h"
#include "../server/Server.h"
#include "../generators/RegularGenerator.h"
#include "../generators/DirectoryGenerator.h"
#include "../generators/ScriptGenerator.h"

string Request::handle(const string &request_data, const char ip[INET_ADDRSTRLEN]) noexcept {
    m_request_data = request_data;
    m_ip = ip;
    string error_message;

    // Parse HTTP request data
    parse();

    // Invalid HTTP request
    if (!m_file.path.is_valid() || m_version.empty() || m_method == HttpConstants::METHOD_ERROR) {
        m_code = HttpConstants::CODE_BAD_REQUEST;
        return get_response();
    }

    // Unknown HTTP version
    if (m_version != "HTTP/1.1") {
        m_code = HttpConstants::CODE_HTTP_VERSION;
        return get_response();
    }

    // Unknown HTTP method
    if (m_method == HttpConstants::METHOD_UNKNOWN) {
        m_code = HttpConstants::CODE_NOT_IMPLEMENTED;
        return get_response();
    }

    // Shutdown requested
    if (m_file.path.get_http() == m_config->find_setting_val("off_address")) {
        raise(SIGTERM);
        return "";
    }

    // Nonexistent file
    if (!m_file.path.exists()) {
        m_code = HttpConstants::CODE_NOT_FOUND;
        return get_response();
    }

    // Check cache
    switch (m_cache->check_file(m_file.path.get_absolute(), m_file.etag)) {
        case Cache::ERROR:
            error_message = m_file.path.get_absolute() + ": unable to check file cache status";
            m_logger->log_message(Logger::ERROR, error_message);
            break;
        case Cache::OK:
            m_code = HttpConstants::CODE_NOT_MODIFIED;
            return get_response();
        case Cache::NOT_FOUND:
            break;
    }

    construct_body();
    return get_response();
}

void Request::reset() noexcept {
    m_response->reset();
    m_request_data.clear();
    m_ip.clear();
    m_method = HttpConstants::METHOD_ERROR;
    m_version.clear();
    m_file.path.clear();
    m_file.mime.clear();
    m_file.etag.clear();
    m_code.clear();
}

string Request::get_response() noexcept {
    string response;

    m_response->set_method(m_method);
    m_response->set_code(m_code);

    // Valid request mime type
    if (m_code == HttpConstants::CODE_OK)
        m_response->set_header("Content-Type", m_file.mime);

    // Set cache control headers
    if (m_cache->get_time() == 0)
        m_response->set_header("Cache-Control", "no-store");
    else {
        string cache_control_val = "public, max-age=" + to_string(m_cache->get_time());
        m_response->set_header("Cache-Control", cache_control_val);
        if (!m_file.etag.empty())
            m_response->set_header("ETag", m_file.etag);
    }

    response = m_response->construct();
    m_logger->log_http(m_request_data, response, m_ip);

    return response;
}

unique_ptr<Generator> Request::get_generator() {
    unique_ptr<Generator> generator = nullptr;

    // Script requested
    if (m_file.path.get_extension() == ".sh") {
        generator = make_unique<ScriptGenerator>(m_file.path);
        return generator;
    }

    // Directory requested
    if (m_file.path.is_directory()) {
        m_file.mime = "text/html";
        Path index(m_file.path.get_absolute(), "/index.html");
        if (index.exists() && index.is_regular()) {
            generator = make_unique<RegularGenerator>(index);
            return generator;
        }
        generator = make_unique<DirectoryGenerator>(m_file.path);
        return generator;
    }

    // Regular file requested
    if (m_file.path.is_regular())
        generator = make_unique<RegularGenerator>(m_file.path);

    return generator;
}

void Request::construct_body() noexcept {
    bool is_text_file = true;
    unique_ptr<Generator> generator = nullptr;
    string error_message = m_file.path.get_absolute() + ": ";

    // Get generator
    try {
        generator = get_generator();
    } catch (const exception& e) {
        error_message += e.what();
        m_logger->log_message(Logger::ERROR, error_message);
        m_code = HttpConstants::CODE_NOT_FOUND;
        return;
    }
    if (generator == nullptr) {
        m_code = HttpConstants::CODE_INTERNAL_ERROR;
        return;
    }

    // Get response body
    try {
        m_response->set_body(generator->get_body(is_text_file));
    } catch (const runtime_error& e) {
        error_message += e.what();
        m_logger->log_message(Logger::ERROR, error_message);
        m_code = HttpConstants::CODE_NOT_FOUND;
        return;
    }

    // Set mime of text file without extension
    if (m_file.mime == "application/octet-stream" && is_text_file)
        m_file.mime = "text/plain";

    m_code = HttpConstants::CODE_OK;

    // Add new file to cache
    if (m_method == HttpConstants::METHOD_GET) {
        if (!m_cache->add_file(m_file.path.get_absolute(), m_file.etag)) {
            error_message += "unable to add file to cache";
            m_logger->log_message(Logger::ERROR, error_message);
        }
    }

    return;
}

void Request::set_mime(const string &extension) noexcept {
    auto mimes_itr = Server::mimes.find(extension);
    if (mimes_itr == Server::mimes.end()) {
        m_file.mime = "application/octet-stream";
        return;
    }
    m_file.mime = mimes_itr->second;
    return;
}

bool Request::extract_header(const string &header, string &destination) noexcept {
    // Find start position of header value (add 2 for ': ')
    // Example -> header: "value"\r\n
    auto pos_start = m_request_data.find(header);
    if (pos_start == string::npos)
        return false;
    pos_start += 2 + header.length();

    // Find position of header value end
    auto pos_end = m_request_data.find("\r\n", pos_start);
    if (pos_end == string::npos)
        return false;
    pos_end -= pos_start;

    destination = m_request_data.substr(pos_start, pos_end);

    return true;
}

void Request::parse_status_line () noexcept {
    // Line looks like "GET /index.html HTTP/1.1\r\n"
    string word;
    istringstream request_stream(m_request_data);

    // Extract request method
    request_stream >> word;
    if (word == "GET")
        m_method = HttpConstants::METHOD_GET;
    else if (word == "HEAD")
        m_method = HttpConstants::METHOD_HEAD;
    else if (word == "POST" || word == "PUT" || word == "DELETE" || word == "CONNECT"
            || word == "OPTIONS" || word == "TRACE" || word == "PATCH")
        m_method = HttpConstants::METHOD_UNKNOWN;
    else
        m_method = HttpConstants::METHOD_ERROR;

    // Extract request path
    if (request_stream.fail() || request_stream.eof())
        return;
    request_stream >> word;
    m_file.path = word;

    // Extract HTTP version
    request_stream.clear();
    if (request_stream.fail() || request_stream.eof())
        return;
    request_stream >> word;
    m_version = word;

    return;
}

void Request::parse() noexcept {
    parse_status_line();
    decode_url(m_file.path.get_http());
    set_mime(m_file.path.get_extension());
    extract_header("If-None-Match", m_file.etag);
    return;
}

void Request::decode_url(const string &url) noexcept {
    // Adjusted code from https://www.rosettacode.org/wiki/URL_decoding#C
    string path_buffer;
    unsigned int hex = 0, len = url.length();
    char c = 0;

    for (size_t i = 0; i < len; i++) {
        // Space
        if (url[i] == '+') {
            path_buffer += ' ';
            continue;
        }
        // Decode % symbol
        if (url[i] == '%') {
            if (!isxdigit(url[i + 1]) || !isxdigit(url[i + 2])) {
                m_file.path = "";
                return;
            } else {
                sscanf(url.substr(i+1, 2).c_str(), "%x", &hex);
                c = static_cast<char>(hex);
                path_buffer += c;
                i += 2;
                continue;
            }
        }
        // Regular symbol
        path_buffer += url[i];
    }

    m_file.path = path_buffer;
    return;
}