//
// Created by satopja2 on 12.03.20.
//

#ifndef EIRSERVER_DIRECTORY_GENERATOR_H
#define EIRSERVER_DIRECTORY_GENERATOR_H

#include "Generator.h"

using namespace std;

/**
 * Generator type class generating response body for directories.
 */
class DirectoryGenerator: public Generator {
    public:
        /**
         * Calls Generator().
         * @param[in] path Absolute path to file from which we generate body.
         * @see Generator
         */
        DirectoryGenerator(const Path &path): Generator(path) {}
        /**
         * Generates HTML listing for given directory.
         * @param[in] is_text_file Ignored.
         * @return String containing data to be appended to HTTP response body.
         * @throw runtime_error If it cannot open or close the directory.
         * @see append_anchor()
         */
        virtual string get_body(bool &is_text_file) override;
    private:
        /**
         * Appends HTML anchor to m_body constructed from given directory entry.
         * @param[in] dir_entry Pointer to directory entry.
         * @throw runtime_error If it cannot check status of the given file.
         */
        void append_anchor(struct dirent *dir_entry);
};


#endif //EIRSERVER_DIRECTORY_GENERATOR_H
