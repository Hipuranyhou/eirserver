//
// Created by satopja2 on 12.03.20.
//

#ifndef EIRSERVER_GENERATOR_H
#define EIRSERVER_GENERATOR_H

#include <string>

#include "../server/Path.h"

using namespace std;

/**
 * Abstract class for response body generators.
 */
class Generator {
    public:
        /**
         * Sets path to file from which we generate HTTP response body (m_path).
         * @param[in] path Absolute path to file from which we generate body.
         */
        Generator(const Path &path): m_path(path) {}
        /**
         * Default destructor.
         */
        virtual ~Generator() = default;
        /**
         * Pure virtual function. Gets body data from file in m_path.
         * @return String containing data to be appended to HTTP response body.
         */
        virtual string get_body(bool &is_text_file) = 0;
    protected:
        /** Member holding HTTP response body data. */
        string m_body;
        /** Absolute path to file from which we generate body. */
        Path m_path;
};


#endif //EIRSERVER_GENERATOR_H
