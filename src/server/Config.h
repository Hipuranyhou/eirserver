//
// Created by satopja2 on 11.03.20.
//

#ifndef EIRSERVER_CONFIG_H
#define EIRSERVER_CONFIG_H

#include <string>
#include <map>

using namespace std;

/**
 * Class parsing and storing server configuration.
 */
class Config {
    public:
        /**
         * Sets m_path and default settings. Tries to parse config file and check if it is valid.
         * @param[in] path %Path to config file.
         * @throw runtime_error If config file is not correct.
         * @see set_default_settings()
         * @see parse()
         * @see check_config()
         */
        Config(const string &path);
        /**
         * Finds setting value of the given key.
         * @param[in] key Key of searched setting.
         * @return String containing setting value.
         */
        string find_setting_val(const string &key) const noexcept;
    private:
        /** Member map storing all settings. */
        map<string, string> m_settings;
        /** Member storing path to config file. */
        string m_path;
        /**
         * Sets \ref Configuration "default Eirserver settings".
         * @throw runtime_error If unable to get current working directory.
         */
        void set_default_settings();
        /**
         * Tries to parse config file line by line and assigning proper settings values.
         * @throw runtime_error If config file is not correct.
         * @see \ref Configuration
         */
        void parse();
        /**
         * Assign value to setting with the name of key.
         * @param[in] key Assigned setting key.
         * @param[in] value Assigned setting value.
         * @return false if given key does not exist, true otherwise
         */
        bool assign_value(const string &key, const string &value) noexcept;
        /**
         * Calls check function for every setting.
         * @throw runtime_error If one of the settings is not correct.
         * @see \ref Configuration
         */
        void check_config() const;
        /**
         * Checks if port is valid int value between 0 and 65535.
         * @param[in] port port value from config file.
         * @throw runtime_error If port is not valid.
         * @see \ref Port "port"
         */
        void check_port(const string &port) const;
        /**
         * Checks if root_dir exists and is directory.
         * @param[in] root_dir root_dir value from config file.
         * @throw runtime_error If root_dir is not valid.
         * @see \ref Root "root_dir"
         */
        void check_root_dir(const string &root_dir) const;
        /**
         * Checks if cache_time is not negative value;
         * @param[in] cache_time cache_time value from config file.
         * @throw runtime_error If cache_time is not valid.
         * @see \ref CacheTime "cache_time"
         */
        void check_cache_time(const string &cache_time) const;
        /**
         * Checks if verbosity is valid choice.
         * @param[in] verbosity verbosity value from config file.
         * @throw runtime_error If verbosity is not valid.
         * @see \ref Verbosity "verbosity"
         */
        void check_verbosity(const string &verbosity) const;
        /**
         * Checks if log_type is valid choice.
         * @param[in] log_type log_type value from config file.
         * @throw runtime_error If log_type is not valid.
         * @see \ref LogType "log_type"
         */
        void check_log_type(const string &log_type) const;
        /**
         * Checks if log_file is not empty when log_type is set to file.
         * @param[in] log_file log_file value from config file.
         * @param[in] log_type log_type value from config file.
         * @throw runtime_error If log_file is not valid.
         * @see \ref LogFile "log_file"
         */
        void check_log_file(const string &log_file, const string &log_type) const;
        /**
         * Checks if off_address starts with /.
         * @param[in] off_address off_address value from config file.
         * @throw runtime_error If off_address is not valid.
         * @see \ref Shutdown "off_adress"
         */
        void check_off_address(const string &off_address) const;
};


#endif //EIRSERVER_CONFIG_H
