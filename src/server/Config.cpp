//
// Created by satopja2 on 11.03.20.
//

#include <fstream>
#include <algorithm>
#include <iterator>
#include <stdexcept>

#include "Config.h"
#include "Path.h"

Config::Config(const string &path) {
    m_path = path;

    try {
        // Load default settings
        set_default_settings();
        if (m_path.empty())
            return;
        // Try to parse config file
        parse();
        check_config();
    } catch (const exception& e) {
        throw runtime_error(e.what());
    }

    return;
}

string Config::find_setting_val(const string &key) const noexcept {
    auto settings_itr = m_settings.find(key);
    if (settings_itr == m_settings.end())
        return "";
    return settings_itr->second;
}

void Config::set_default_settings() {
    m_settings = {
            {"ip", "0.0.0.0"},
            {"port", "8080"},
            {"cache_time", "3600"},
            {"verbosity", "minimal"},
            {"log_type", "console"},
            {"log_file", ""},
            {"off_address", "/shutdown"},
    };
    m_settings["root_dir"] = get_current_directory();
    return;
}

void Config::parse() {
    string line, key, value;
    ifstream file(m_path);

    // Try to open config file
    if (!file.is_open())
        throw runtime_error("opening file failed");

    // Read key = value pairs
    while (getline(file, line)) {
        if (file.fail())
            throw runtime_error("reading file failed");
        file.clear();
        // Erase white space characters from line (key = value -> key=value)
        line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end());
        if (line.empty() || line.front() == '#')
            continue;
        key = line.substr(0, line.find('='));
        value = line.substr(line.find('=') + 1);
        if (key.empty() || value.empty()) {
            file.clear();
            file.close();
            throw runtime_error("missing key or value");
        }
        if (!assign_value(key, value))
            throw runtime_error("unknown option");
    }

    // Try to close config file
    file.clear();
    file.close();

    return;
}

bool Config::assign_value(const string &key, const string &value) noexcept {
    auto settings_itr = m_settings.find(key);
    if (settings_itr == m_settings.end())
        return false;
    settings_itr->second = value;
    return true;
}

void Config::check_config() const {
    try {
        check_port(find_setting_val("port"));
        check_root_dir(find_setting_val("root_dir"));
        check_cache_time(find_setting_val("cache_time"));
        check_verbosity(find_setting_val("verbosity"));
        check_log_type(find_setting_val("log_type"));
        check_log_file(find_setting_val("log_file"), find_setting_val("log_type"));
        check_off_address(find_setting_val("off_address"));
    } catch (const runtime_error& e) {
        throw runtime_error(e.what());
    }
    return;
}

void Config::check_port(const string &port) const {
    try {
        int port_number = stoi(port);
        if (port_number < 0 || port_number > 65535)
            throw runtime_error("port has to be >= 0 and <= 65535");
    } catch (const exception& e) {
        throw runtime_error("port is invalid");
    }
    return;
}

void Config::check_root_dir(const string &root_dir) const {
    try {
        Path root(root_dir);
        if (!root.exists())
            throw runtime_error("root_dir does not exist");
        if (!root.is_directory())
            throw runtime_error("root_dir is not a directory");
    } catch (const system_error& e) {
        throw runtime_error("encountered filesystem error while checking root_dir");
    }
    return;
}

void Config::check_cache_time(const string &cache_time) const {
    try {
        int cache_time_number = stoi(cache_time);
        if (cache_time_number < 0)
            throw runtime_error("cache_time has to be >= 0");
    } catch (const logic_error& e) {
        throw runtime_error("cache_time is invalid");
    }
    return;
}

void Config::check_verbosity(const string &verbosity) const {
    if (verbosity != "none" && verbosity != "minimal" && verbosity != "verbose")
        throw runtime_error("verbosity option is invalid");
    return;
}

void Config::check_log_type(const string &log_type) const {
    if (log_type != "console" && log_type != "syslog" && log_type != "file")
        throw runtime_error("log_type option is invalid");
    return;
}

void Config::check_log_file(const string &log_file, const string &log_type) const {
    if (log_file.empty() && log_type == "file")
        throw runtime_error("missing log_file path");
    return;
}

void Config::check_off_address(const string &off_address) const {
    if (off_address.empty() || off_address.front() != '/')
        throw runtime_error("off_address has to start with '/'");
    return;
}
















