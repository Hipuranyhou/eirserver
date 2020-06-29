//
// Created by satopja2 on 28.03.20.
//

#include <ctime>
#include <sstream>

#include "Logger.h"

string Logger::get_date() noexcept {
    string date;
    time_t time_obj = time(nullptr);
    char date_c_str[20];

    // Get current date and time
    strftime(date_c_str, sizeof(date_c_str), "%d.%m.%Y %T", localtime(&time_obj));
    date.append(date_c_str);

    return date;
}

void Logger::set_log_type(const log_types &type) noexcept {
    switch (type) {
        case ERROR:
            m_body = "  ERROR ";
            break;
        case WARNING:
            m_body = "WARNING ";
            break;
        case INFO:
            m_body = "   INFO ";
            break;
    }
    return;
}

string Logger::extract_request_status(const string &request) noexcept {
    string line;
    istringstream request_stream(request);
    getline(request_stream, line, '\r');
    return line;
}

void Logger::append_request_headers(const string &request, const int &format_width) noexcept {
    string line;
    istringstream request_stream(request);

    // Skip first request line which we already included
    getline(request_stream, line);

    // Apend title
    m_body.append(format_width, ' ');
    m_body += "##### REQUEST HEADERS #####\n";

    // Append headers
    while (getline(request_stream, line)) {
        if (iscntrl(line[0]))
            continue;
        m_body.append(format_width, ' ');
        m_body += line;
        // Erase '\r' at end of line
        m_body.erase(m_body.end()-1);
        m_body += "\n";
    }

    return;
}

string Logger::extract_response_code(const string &response) noexcept {
    // Example -> "HTTP/1.1 200 Ok\r\n"
    string response_code;
    int i = 0;

    // Start of code is always at index 9 (1st line has always minimally "HTTP/1.1 \r\n")
    while(response[i+9] != '\r') {
        response_code.append(response, i+9, 1);
        i++;
    }

    return response_code;
}

void Logger::append_response_headers(const string &response, const int &format_width) noexcept {
    string line;
    istringstream response_stream(response);

    // Skip first response line which we already included
    getline(response_stream, line);

    // Append title
    m_body.append(format_width, ' ');
    m_body += "##### RESPONSE HEADERS #####\n";

    // Append headers
    while (getline(response_stream, line)) {
        if (iscntrl(line[0]))
            break;
        m_body.append(format_width, ' ');
        m_body += line;
        // Erase '\r' at end of line
        m_body.erase(m_body.end()-1);
        m_body += "\n";
    }

    return;
}

void Logger::construct_body(const string &request, const string &response, const string &ip) noexcept {
    int format_width = 0;

    // Set minimal HTTP log m_body
    m_body = "   HTTP [" + get_date() + "]: ";
    format_width = m_body.length();
    m_body += ip + " - \"" + extract_request_status(request) + "\" <- \"" + extract_response_code(response) + "\"\n";

    if (m_verbosity == "verbose") {
        append_request_headers(request, format_width);
        append_response_headers(response, format_width);
    }

    return;
}