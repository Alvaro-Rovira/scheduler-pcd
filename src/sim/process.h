#pragma once

#include <algorithm>
#include <cstddef>
#include <string>
#include <vector>

enum class State { NEW, READY, RUNNING, BLOCKED, TERMINATED };

struct Process {
    int pid = 0;
    std::string name;
    int arrival = 0;
    int priority = 0;
    std::vector<int> bursts;

    State state = State::NEW;
    std::size_t burst = 0;
    int remaining = 0;

    int firstRun = -1;
    int finish = -1;
    int waiting = 0;
    std::string timeline;

    int total(bool cpu) const {
        int sum = 0;
        for (std::size_t i = cpu ? 0 : 1; i < bursts.size(); i += 2) sum += bursts[i];
        return sum;
    }
};

inline std::size_t longestName(const std::vector<Process>& procs, std::size_t minimum) {
    std::size_t w = minimum;
    for (const Process& p : procs) w = std::max(w, p.name.size());
    return w;
}
