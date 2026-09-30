#pragma once

#include <string>
#include <vector>

#include "sim/process.h"

std::vector<Process> defaultProcesses();
std::vector<Process> loadProcesses(const std::string& path);
void printWorkload(const std::vector<Process>& procs);
