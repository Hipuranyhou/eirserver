//
// Created by satopja2 on 12.03.20.
//

#ifndef EIRSERVER_REGULAR_GENERATOR_H
#define EIRSERVER_REGULAR_GENERATOR_H

#include "Generator.h"

using namespace std;

/**
 * Generator type class generating response body for regular files.
 */
class RegularGenerator: public Generator {
    public:
        /**
         * Calls Generator().
         * @param[in] path Absolute path to file from which we generate body.
         * @see Generator
         */
        RegularGenerator(const Path &path): Generator(path) {}
        /**
         * Reads data from file as if it was binary file. It tries to guess if the file is binary or text
         * and sets is_text_file appropriately.
         * @return String containing data to be appended to HTTP response body.
         * @throw runtime_error If it cannot access the file.
         * @note It tries to guess if file is text or binary by looking at first m_is_text_range bytes and checking if
         * it contains null bytes. Same heuristic is used for example by grep and less.
         */
        virtual string get_body(bool &is_text_file) override;
    private:
        /** Member holding how many bytes should be checked while guessing if given file is binary or text. */
        const size_t m_is_text_range = 1024;
};


#endif //EIRSERVER_REGULAR_GENERATOR_H
