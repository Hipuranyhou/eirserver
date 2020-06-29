//
// Created by satopja2 on 08.03.20.
//

#include <iostream>
#include <string>
#include <unistd.h>
#include <stdexcept>
#include <netinet/in.h>

#include "server/Server.h"

using namespace std;

int main(int argc, char *argv[]) {
    int option;
    string config_path;

    // Process command line options
    while ((option = getopt(argc, argv, ":c:")) != -1) {
        switch (option) {
            case 'c':
                config_path = optarg;
                break;
            case ':':
                cerr << "Flag -c needs path to config file!" << endl;
                cerr << "Usage: " << argv[0] << " -c \"path/to/config/file\"" << endl;
                return 1;
            case '?':
                cerr << "Unknown flag -" << option << endl;
                return 1;
        }
    }

    // Try to start Eirserver
    try {
        Server eirserver(config_path);
        if (!eirserver.start())
            return 1;
    } catch (const exception &e) {
        cerr << "Error encountered while setting up Eirserver" << endl;
        cerr << e.what() << endl;
        return 1;
    }

    return 0;
}