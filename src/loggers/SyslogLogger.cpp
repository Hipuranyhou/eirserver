//
// Created by satopja2 on 28.03.20.
//

#include <syslog.h>

#include "SyslogLogger.h"

SyslogLogger::SyslogLogger(const string &verbosity) {
    m_verbosity = verbosity;
    if (m_verbosity == "none")
        return;

    openlog("Eirserver", LOG_PID, LOG_USER);
    log_message(WARNING, "Starting Eirserver!");

    return;
}

SyslogLogger::~SyslogLogger() {
    if (m_verbosity == "none")
        return;

    log_message(WARNING, "Closing Eirserver!");
    closelog();

    return;
}

void SyslogLogger::log_message(const log_types &type, const string &message) noexcept {
    if (m_verbosity == "none")
        return;

    // Prepare m_body
    set_log_type(type);
    m_body = message;

    // Log m_body
    syslog(m_type, "%s", m_body.c_str());

    return;
}

void SyslogLogger::log_http(const string &request, const string &response, const string &ip) noexcept {
    if (m_verbosity == "none")
        return;

    // Prepare and log m_body
    m_body = "HTTP: " + ip + " - \"" + extract_request_status(request) + "\" <- \"" + extract_response_code(response) + "\"\n";
    syslog(m_type, "%s", m_body.c_str());

    return;
}

void SyslogLogger::set_log_type(const log_types &type) noexcept {
    switch (type) {
        case ERROR:
            m_type = LOG_ERR;
            break;
        case WARNING:
            m_type = LOG_WARNING;
            break;
        case INFO:
            m_type = LOG_INFO;
            break;
    }
    return;
}
