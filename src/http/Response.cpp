//
// Created by satopja2 on 10.03.20.
//

#include <ctime>

#include "Response.h"

void Response::set_header(const string &header, const string &value) noexcept {
    m_headers.insert_or_assign(header, value);
    return;
}

void Response::set_method(const HttpConstants::http_methods &method) noexcept {
    m_method = method;
    return;
}

void Response::set_code(const string &code) noexcept {
    m_code = code;
    return;
}

void Response::set_body(const string &data) noexcept {
    m_body = data;
    return;
}

string Response::construct() noexcept {
    string response;

    get_date();

    response = "HTTP/1.1 " + m_code + "\r\n";
    response += "Server: Eirserver\r\n";
    response += "Date: " + get_date() + "\r\n";

    // Append headers
    for (auto const& header : m_headers)
        response += header.first + ": " + header.second + "\r\n";

    if (!m_body.empty())
        response += "Content-Length: " + to_string(m_body.length()) + "\r\n";

    response += "\r\n";

    // Append body
    if (m_method != HttpConstants::METHOD_HEAD && m_code == HttpConstants::CODE_OK)
        response += m_body;

    return response;
}

void Response::reset() noexcept {
    m_headers.clear();
    m_method = HttpConstants::METHOD_UNKNOWN;
    m_code.clear();
    m_body.clear();
    return;
}

string Response::get_date() noexcept {
    string date;
    char date_c_str[30];
    time_t time_obj = time(nullptr);

    // Get current date and time
    strftime(date_c_str, sizeof(date_c_str), "%a, %d %b %Y %T GMT", gmtime(&time_obj));
    date.append(date_c_str);

    return date;
}