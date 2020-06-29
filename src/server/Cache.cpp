//
// Created by satopja2 on 17.03.20.
//

#include <iterator>
#include <sys/stat.h>
#include <functional>

#include "Cache.h"

Cache::cache_status Cache::check_file(const string &path, const string &etag) noexcept {
    struct stat path_status;
    time_t now = std::time(nullptr);
    auto entries_itr = m_entries.find(path);

    // Disabled cache
    if (m_time == 0)
        return NOT_FOUND;

    // File not found in cache
    if (entries_itr == m_entries.end())
        return NOT_FOUND;

    // Delete too old cache entry
    if ((now - entries_itr->second.access) > m_time) {
        m_entries.erase(entries_itr);
        return NOT_FOUND;
    }

    // Get file info
    if (stat(path.c_str(), &path_status) < 0)
        return ERROR;

    // Found valid cache entry
    if (entries_itr->second.last_mod == path_status.st_mtime && entries_itr->second.etag == etag) {
        entries_itr->second.access = now;
        return OK;
    }

    // File found but changed
    m_entries.erase(entries_itr);
    return NOT_FOUND;
}

bool Cache::add_file(const string &path, string &etag) noexcept {
    struct stat path_status;
    time_t now = std::time(nullptr);

    // Disabled cache
    if (m_time == 0)
        return true;

    // Get file info
    if (stat(path.c_str(), &path_status) < 0)
        return false;

    // Set etag of file and add it to cache
    etag = path;
    etag += to_string(path_status.st_mtime);
    etag = "\"" + to_string(hash<string>{}(etag)) + "\"";
    m_entries[path] = file(etag, path_status.st_mtime, now);

    return true;
}

int Cache::get_time() noexcept {
    return m_time;
}