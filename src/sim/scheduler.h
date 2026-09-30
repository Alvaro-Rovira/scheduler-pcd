#pragma once

#include <deque>
#include <string>
#include <vector>

#include "sim/config.h"
#include "sim/process.h"

class Scheduler {
public:
    Scheduler(std::vector<Process> processes, Config cfg);

    void run();
    void printTimeline() const;
    void printStats() const;
    Summary summary() const;

private:
    void admitArrivals();
    void wakeBlocked();
    void releaseCpu();
    void dispatch();
    void execute();

    std::deque<Process*>::iterator pickNext();
    bool beats(const Process* candidate, const Process* current) const;
    bool isPreemptive() const;
    bool allTerminated() const;

    void toReady(Process* p);
    void start(Process* p);
    void event(const std::string& text);

    void printTraceHeader() const;
    void printTraceLine();

    std::vector<Process> procs_;
    Config cfg_;

    int clock_ = 0;
    std::deque<Process*> ready_;
    std::vector<Process*> blocked_;
    Process* running_ = nullptr;
    Process* incoming_ = nullptr;
    Process* lastOnCpu_ = nullptr;
    int switchLeft_ = 0;
    int sliceUsed_ = 0;

    int busyTicks_ = 0;
    int switchTicks_ = 0;
    int idleTicks_ = 0;
    int contextSwitches_ = 0;
    std::string cpuLine_;
    std::vector<std::string> events_;

    int cpuWidth_ = 3;
    int readyWidth_ = 5;
    int blockedWidth_ = 7;
};
