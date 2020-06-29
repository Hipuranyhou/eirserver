//
// Created by satopja2 on 12.03.20.
//

#include <fstream>
#include <sstream>
#include <vector>
#include <stdexcept>

#include "RegularGenerator.h"

string RegularGenerator::get_body(bool &is_text_file) {
    ifstream file;
    ostringstream buffer;

    // Prepare file for reading
    file.open(m_path.get_absolute(), ios::in | ios::binary);
    if (!file.is_open())
        throw runtime_error("unable to open file");

    // Load file
    buffer << file.rdbuf();
    m_body = buffer.str();

    // Guess if the file is binary or not based on m_is_text_range bytes
    size_t len = (m_body.length() < m_is_text_range) ? m_body.length() : m_is_text_range;
    for (size_t i = 0; i < len; ++i) {
        if (m_body[i] == 0) {
            is_text_file = false;
            break;
        }
    }

    // Close file
    file.clear();
    file.close();
    if (file.fail())
        throw runtime_error("unable to close file");

    return m_body;
}