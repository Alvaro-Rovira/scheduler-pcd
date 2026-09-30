#pragma once

#include <vector>

#include "sim/config.h"
#include "sim/process.h"

void simulate(const std::vector<Process>& procs, const Config& cfg);
void compare(const std::vector<Process>& procs, Config base);
