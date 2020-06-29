//
// Created by satopja2 on 23.04.20.
//

#include <cerrno>
#include <unistd.h>
#include <sys/types.h>
#include <stdexcept>

#include "Path.h"

string get_current_directory() {
    string path;
    int size = 256;
    // We need to use malloc and realloc because getcwd does not like memory allocated with new
    char *buffer = (char*)malloc(size * sizeof(*buffer)), *getcwd_val = NULL;

    // We need to check for current directory in a loop until buffer is big enough
    while (!getcwd_val) {
        getcwd_val = getcwd(buffer, size);
        if (!getcwd_val && errno != ERANGE) {
            // Other error than buffer not big enough
            free(buffer);
            throw runtime_error("unable to get current working directory");
        }
        // Resize buffer
        size *= 2;
        if ((buffer = (char*)realloc(buffer, size * sizeof(*buffer))) == NULL)
            throw runtime_error("unable to get current working directory");
    }

    path = buffer;
    free(buffer);

    return path;
}

Path::Path(const string &root) {
    if (root.empty())
        throw invalid_argument("path to root directory is empty");

    // Set root directory for non absolute root
    if (root.front() != '/') {
        try {
            m_root = get_current_directory();
        } catch (const exception &e) {
            throw runtime_error(e.what());
        }
    }

    m_root += "/" + root;
    m_http = "";
    set_absolute();
    return;
}

Path::Path(const string &root, const string &http) {
    if (root.empty() || root.front() != '/')
        throw invalid_argument("path to root directory is empty");

    // Set root directory for non absolute root
    if (root.front() != '/') {
        try {
            m_root = get_current_directory();
        } catch (const exception &e) {
            throw runtime_error(e.what());
        }
    }

    m_root += "/" + root;
    m_http = http;
    set_absolute();
    return;
}

Path& Path::operator =(const string &path) noexcept {
    m_http = path;
    set_absolute();
    return *this;
}

bool Path::exists() const {
    return (get_status() == 0);
}

bool Path::is_directory() const {
    int stat_val = get_status();

    if (stat_val < 0 && errno == ENOENT)
        throw runtime_error("no such file or directory");
    if (stat_val < 0 && errno == EACCES)
        throw runtime_error("permission denied");
    if (stat_val < 0)
        throw runtime_error("unable to check file");

    return S_ISDIR(m_status.st_mode);
}

bool Path::is_regular() const {
    int stat_val = get_status();

    if (stat_val < 0 && errno == ENOENT)
        throw runtime_error("no such file or directory");
    if (stat_val < 0 && errno == EACCES)
        throw runtime_error("permission denied");
    if (stat_val < 0)
        throw runtime_error("unable to check file");

    return S_ISREG(m_status.st_mode);
}

bool Path::is_valid() const noexcept {
    // Check for path traversal
    auto pos_traversal = m_http.find("/../");
    if (pos_traversal != string::npos)
        return false;

    // Check if requested path is not empty and is absolute
    return (!m_http.empty() && m_http.front() == '/');
}

string Path::get_http() const noexcept {
    return m_http;
}

string Path::get_absolute() const noexcept {
    return m_absolute;
}

string Path::get_extension() const noexcept {
    string filename = get_filename();

    // Files '.' and '..'
    if (filename == "." || filename == "..")
        return "";

    // File without extension
    auto pos_dot = filename.find_last_of('.');
    if (pos_dot == string::npos)
        return "";

    // Hidden file without extension
    if (pos_dot == 0)
        return "";

    return filename.substr(pos_dot);
}

string Path::get_filename() const noexcept {
    auto pos_separator = m_http.find_last_of('/');
    // Invalid HTTP path
    if (pos_separator == string::npos)
        return ""

    // No filename
    if ((pos_separator + 1) == m_http.size())
        return "";

    return m_http.substr(pos_separator + 1);
}

string Path::get_root() const noexcept {
    return m_root;
}

void Path::clear() noexcept {
    m_http.clear();
    m_absolute = m_http;
    return;
}

int Path::get_status() const noexcept {
    return stat(m_absolute.c_str(), &m_status);
}

void Path::set_absolute() noexcept {
    m_absolute = m_root + m_http;
    return;
}