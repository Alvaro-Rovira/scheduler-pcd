#pragma once

#include <string>

enum class Algorithm { FCFS, SJF, RR, PRIORITY };

struct Config {
    Algorithm algorithm = Algorithm::RR;
    int quantum = 3;
    bool preemptive = false;
    int contextSwitch = 1;
    bool verbose = true;
};

struct Summary {
    double avgTurnaround = 0;
    double avgWaiting = 0;
    double avgResponse = 0;
    double cpuUsage = 0;
    int totalTime = 0;
    int contextSwitches = 0;
};

std::string algorithmName(const Config& cfg);
std::string preemptionLabel(const Config& cfg);
