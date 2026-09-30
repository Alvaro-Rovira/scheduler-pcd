#pragma once

#include <string>

#include "sim/config.h"

struct Options {
    Config cfg;
    std::string file;
    bool compare = false;
    bool help = false;
};

Options parseArgs(int argc, char** argv);
void printUsage();
