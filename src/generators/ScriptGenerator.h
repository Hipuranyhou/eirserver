//
// Created by satopja2 on 12.03.20.
//

#ifndef EIRSERVER_SCRIPT_GENERATOR_H
#define EIRSERVER_SCRIPT_GENERATOR_H

#include "Generator.h"

using namespace std;

/**
 * Generator type class generating response body for shell scripts.
 */
class ScriptGenerator: public Generator {
    public:
        /**
         * Calls Generator().
         * @param[in] path Absolute path to file from which we generate body.
         * @see Generator
         */
        ScriptGenerator(const Path &path): Generator(path) {}
        /**
         * Opens pipe to given shell script and reads its output.
         * @param[in] is_text_file Ignored.
         * @return String containg data to be appended to HTTP response body.
         * @throw runtime_error If the pipe cannot be opened.
         */
        virtual string get_body(bool &is_text_file) override;
};


#endif //EIRSERVER_SCRIPT_GENERATOR_H
