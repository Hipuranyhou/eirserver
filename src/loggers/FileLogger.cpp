//
// Created by satopja2 on 28.03.20.
//

#include <syslog.h>
#include <stdexcept>

#include "FileLogger.h"

FileLogger::FileLogger(const string &verbosity, const string &log_file) {
    m_verbosity = verbosity;
    if (m_verbosity == "none")
        return;

    // Try to open log file
    m_log_file.open(log_file, ios::out | ios::app);
    if (!m_log_file.is_open())
        throw runtime_error("unable to open log file");

    log_message(WARNING, "Starting Eirserver!");

    return;
}

FileLogger::~FileLogger() {
    if (m_verbosity == "none")
        return;

    log_message(WARNING, "Closing Eirserver!");

    // Try to close log file
    m_log_file.clear();
    m_log_file.close();
    if (m_log_file.fail()) {
        openlog("Eirserver", LOG_PID | LOG_PERROR, LOG_USER);
        syslog(LOG_ERR, "%s", "Error while closing log file!");
        closelog();
    }

    return;
}

void FileLogger::log_message(const log_types &type, const string &message) noexcept {
    if (m_verbosity == "none")
        return;

    // Prepare m_body
    set_log_type(type);
    m_body += "[" + get_date() + "]: " + message;

    // Try to log to file
    m_log_file.clear();
    m_log_file << m_body << endl;
    if (m_log_file.fail()) {
        openlog("Eirserver", LOG_PID | LOG_PERROR, LOG_USER);
        syslog(LOG_ERR, "%s", "Error while writing to log file!");
        closelog();
    }

    return;
}

void FileLogger::log_http(const string &request, const string &response, const string &ip) noexcept {
    if (m_verbosity == "none")
        return;

    // Prepare m_body
    construct_body(request, response, ip);

    // Try to log to file
    m_log_file.clear();
    m_log_file << m_body << flush;
    if (m_log_file.fail()) {
        openlog("Eirserver", LOG_PID | LOG_PERROR, LOG_USER);
        syslog(LOG_ERR, "%s", "Error while writing to log file!");
        closelog();
    }

    return;
}