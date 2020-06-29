//
// Created by satopja2 on 12.03.20.
//

#include <cstdio>
#include <stdexcept>

#include "ScriptGenerator.h"

string ScriptGenerator::get_body(bool &is_text_file) {
    FILE* pipe;
    const size_t buffer_size = 512;
    char buffer[buffer_size];
    string exec = "exec " + m_path.get_absolute();

    // Open pipe to script file
    if ((pipe = popen(exec.c_str(), "r")) == NULL)
        throw runtime_error("unable to open pipe");

    // Read script file output
    while (!feof(pipe)) {
        if (ferror(pipe) != 0)
            throw runtime_error("unable to read pipe");
        if (fgets(buffer, buffer_size, pipe) != NULL)
            m_body += buffer;
    }

    // Close pipe
    if (pclose(pipe) == -1)
        throw runtime_error("unable to close pipe");

    return m_body;
}