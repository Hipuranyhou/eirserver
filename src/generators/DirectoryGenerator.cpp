//
// Created by satopja2 on 12.03.20.
//

#include <stdexcept>
#include <dirent.h>

#include "DirectoryGenerator.h"


string DirectoryGenerator::get_body(bool &is_text_file) {
    DIR *directory;
    struct dirent *dir_entry;

    // Try to open directory
    directory = opendir(m_path.get_absolute().c_str());
    if (directory == NULL)
        throw runtime_error("unable to open directory");

    // Prepare HTML of directory listing
    string title = "Index of " + m_path.get_http();
    m_body = "<html><head><meta charset=\"UTF-8\"><title>" + title + "</title></head><body>";
    m_body += "<h1>" + title + "</h1>";

    // Append HTML anchor for every file in directory
    try {
        while ((dir_entry = readdir(directory)))
            append_anchor(dir_entry);
    } catch (const exception& e) {
        throw runtime_error(e.what());
    }

    // Close HTML
    m_body += "</body></html>";

    if (closedir(directory) < 0)
        throw runtime_error("unable to close directory");

    return m_body;
}

void DirectoryGenerator::append_anchor(struct dirent *dir_entry) {
    Path entry(m_path);
    string filename, href, type;

    // Construct absolute path to directory entry
    entry = m_path.get_http() + "/" + dir_entry->d_name;

    // Ignore files '.' and '..'
    filename = entry.get_filename();
    if (filename == "." || filename == "..")
        return;

    // Set different link for server root
    href = (m_path.get_http() == "/") ? ("/" + filename) : (m_path.get_http() + "/" + filename);

    // Set file type in listing
    type = "[other]  ";
    if (entry.is_directory())
        type = "[dir]    ";
    if (entry.is_regular())
        type = "[file]   ";

    // Append anchor to HTML
    m_body += type + "<a href=\"" + href + "\">" + filename + "</a><br/>";

    return;
}