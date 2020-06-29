//
// Created by satopja2 on 28.03.20.
//

#include <iostream>

#include "ConsoleLogger.h"

ConsoleLogger::ConsoleLogger(const string &verbosity) {
    m_verbosity = verbosity;
    if (m_verbosity == "none")
        return;

    log_message(WARNING, "Starting Eirserver!");

    return;
}

ConsoleLogger::~ConsoleLogger() {
    if (m_verbosity == "none")
        return;

    log_message(WARNING, "Closing Eirserver!");

    return;
}

void ConsoleLogger::log_message(const log_types &type, const string &message) noexcept {
    if (m_verbosity == "none")
        return;

    // Prepare m_body
    set_log_type(type);
    m_body += "[" + get_date() + "]: " + message;

    // Log m_body
    if (type == ERROR) {
        cerr << m_body << endl;
        return;
    }
    cout << m_body << endl;

    return;
}

void ConsoleLogger::log_http(const string &request, const string &response, const string &ip) noexcept {
    if (m_verbosity == "none")
        return;

    // Prepare and log m_body
    construct_body(request, response, ip);
    cout << m_body << flush;

    return;
}