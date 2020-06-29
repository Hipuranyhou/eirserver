//
// Created by satopja2 on 17.03.20.
//

#ifndef EIRSERVER_CACHE_H
#define EIRSERVER_CACHE_H

#include <map>
#include <string>
#include <ctime>

using namespace std;

/**
 * Class storing and checking cache entries.
 */
class Cache {
    public:
        /**
         * Sets cache time value (m_time).
         * @param[in] time %Cache time value.
         */
        Cache(const int &time): m_time(time) {}
        /**
         * Member holding cache status responses
         */
        enum cache_status {
            OK,
            NOT_FOUND,
            ERROR
        };
        /**
         * Searches cache for file with given path and ETag value. Deletes too old cache entries. For found file
         * resets its age in cache.
         * @param[in] path %Path of checked cache entry.
         * @param[in] etag ETag value of checked cache entry.
         * @return ERROR if stat() encountered error, OK if file found in cache, NOT_FOUND if file not found in cache
         * or was modified.
         */
        cache_status check_file(const string &path, const string &etag) noexcept;
        /**
         * Adds file to cache.
         * @param[in] path %Path to file added to cache
         * @param[out] etag ETag value of file added to cache.
         * @return true if file added to cache or cache disabled, false otherwise
         */
        bool add_file(const string &path, string &etag) noexcept;
        /**
         * Gets cache time value (m_time).
         * @return Int representing cache time value.
         */
        int get_time() noexcept;
    private:
        /**
         * Struct storing information about cache entries.
         */
        struct file {
            /**
             * Default constructor.
             */
            file(): etag(""), last_mod(0), access(0) {}
            /**
             * Sets etag, last_mod and access.
             * @param[in] etag_val ETag value of cache entry.
             * @param[in] last_mod_val Last modification time of cached file.
             * @param[in] access_val Last cache access time of cache entry.
             */
            file(string etag_val, time_t last_mod_val, time_t access_val) : etag(etag_val), last_mod(last_mod_val), access(access_val) {}
            /** Member containing ETag value of cache entry. */
            string etag;
            /** Member containing last modification time of cached file. */
            time_t last_mod;
            /** Member containing last cache access time of cache entry. */
            time_t access;
        };
        /** Member map storing all cache entries. */
        map<string, struct file> m_entries;
        /** Member holding cache time value (for how long should files be kept in cache). */
        int m_time;
};


#endif //EIRSERVER_CACHE_H
