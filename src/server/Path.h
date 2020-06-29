//
// Created by satopja2 on 23.04.20.
//

#ifndef EIRSERVER_PATH_H
#define EIRSERVER_PATH_H

#include <string>

#include <sys/stat.h>

using namespace std;

/**
 * Helper function to get current working directory (as if pwd was called).
 * @return String containing absolute path of current working directory.
 * @throw runtime_error If getcwd() or realloc() encountered error.
 */
string get_current_directory();

/**
 * Helper class for working with filesystem and HTTP request paths at the same time.
 * @note Even though C++17 filesystem library exists, because we often need to work with the same path
 * sometimes with its absolute path to for example get data from the file or check its status and sometimes with
 * the path from HTTP request to for example check if its valid or generate links absolute against server in browser.
 * Because of this is working with this class easier than for example storing requested path as std::filesystem::path
 * and absolute path as string or vice versa separately.
 * @note And Progtest was protesting against '#include <filesystem>' even though Progtest VM did not.
 */
class Path {
    public:
        /**
         * Constructor setting root directory (m_root) and absolute path (m_absolute) to current working directory.
         * @throw runtime_error If get_current_directory() encountered error.
         */
        Path(): m_root(get_current_directory()), m_http(""), m_absolute(m_root) {}
        /**
         * Constructor checks if root is not empty, and sets m_root to root if root is absolute path and to
         * get_current_directory() + root if root is not absolute path. Sets m_absolute to m_root.
         * @param[in] root %Path used as root directory for filesystem operations.
         * @throw invalid_argument If root path is empty.
         * @throw runtime_error If get_current_directory() encountered error.
         */
        Path(const string &root);
        /**
         * Constructor checks if root is not empty, and sets m_root to root if root is absolute path and to
         * get_current_directory() + root if root is not absolute path. Sets m_absolute to m_root + m_http.
         * @param[in] root %Path used as root directory for filesystem operations.
         * @param[in] http %Path from HTTP request.
         * @throw invalid_argument If root path is empty.
         * @throw runtime_error If get_current_directory() encountered error.
         */
        Path(const string &root, const string &http);
        /**
         * Sets m_http to path.
         * @param[in] path %Path to be appended.
         * @return *this.
         */
        Path& operator =(const string &path) noexcept;
        /**
         * Checks whether given file exists using stat().
         * @return true if file exists, false otherwise.
         * @note We only check if stat() for given file fails, it does not necessarily mean the file
         * does not exits. We could for example not have enough permissions to call stat() on given file, but that for
         * us means, the file does not exist, because as a server we choose to hide forbidden resources from clients as
         * can be seen in documentation here https://tools.ietf.org/html/rfc2616#section-10.4.4 .
         */
        bool exists() const;
        /**
         * Gets status of file and checks whether requested file is a directory using S_ISDIR.
         * @return true if file is a directory, false otherwise
         * @throw runtime_error If stat() encountered error.
         * @see get_status()
         */
        bool is_directory() const;
        /**
         * Gets status of file and checks whether requested file is a directory using S_ISREG.
         * @return true if file is a regular file, false otherwise
         * @throw runtime_error If stat() encountered error.
         * @see get_status()
         */
        bool is_regular() const;
        /**
         * Checks for path traversal ('/../' is not allowed anywhere in m_http), if m_http is not empty and
         * if m_http is absolute path.
         * @return true if path is valid, false otherwise.
         */
        bool is_valid() const noexcept;
        /**
         * Gets path from HTTP request (m_http).
         * @return String containing HTTP path.
         */
        string get_http() const noexcept;
        /**
         * Gets absolute path of requested file (m_absolute).
         * @return String containing absolute path.
         */
        string get_absolute() const noexcept;
        /**
         * Extracts extension of file from m_http.
         * @return String containing extension.
         */
        string get_extension() const noexcept;
        /**
         * Extracts filename of file from m_http.
         * @return String containing filename.
         */
        string get_filename() const noexcept;
        /**
         * Gets root directory of requested file (m_root).
         * @return String containing root directory.
         */
        string get_root() const noexcept;
        /**
         * Clears m_http and sets m_absolute to m_root. Does not clear m_root!
         */
        void clear() noexcept;
    private:
        /** Member holding path to be used as root directory for filesystem operations. */
        string m_root;
        /** Member holding path from HTTP request. */
        string m_http;
        /** Member holding absolute path to requested file. */
        string m_absolute;
        /** Member struct holding information from stat(). */
        mutable struct stat m_status;
        /**
         * Calls stat() on file in m_absolute and struct stat m_status.
         * @return Integer return value of stat()
         */
        int get_status() const noexcept;
        /**
         * Connects m_root + m_http to m_absolute.
         */
        void set_absolute() noexcept;
};

#endif //EIRSERVER_PATH_H