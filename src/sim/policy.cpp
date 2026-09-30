#include <algorithm>

#include "sim/scheduler.h"

std::deque<Process*>::iterator Scheduler::pickNext() {
    switch (cfg_.algorithm) {
        case Algorithm::SJF:
            return std::min_element(ready_.begin(), ready_.end(),
                                    [](const Process* a, const Process* b) { return a->remaining < b->remaining; });
        case Algorithm::PRIORITY:
            return std::min_element(ready_.begin(), ready_.end(),
                                    [](const Process* a, const Process* b) { return a->priority < b->priority; });
        case Algorithm::FCFS:
        case Algorithm::RR:
            break;
    }
    return ready_.begin();
}

bool Scheduler::beats(const Process* candidate, const Process* current) const {
    if (cfg_.algorithm == Algorithm::SJF) return candidate->remaining < current->remaining;
    return candidate->priority < current->priority;
}

bool Scheduler::isPreemptive() const {
    return cfg_.preemptive && (cfg_.algorithm == Algorithm::SJF || cfg_.algorithm == Algorithm::PRIORITY);
}
