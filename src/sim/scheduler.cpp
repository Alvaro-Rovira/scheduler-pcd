#include "sim/scheduler.h"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace {

const int MAX_TICKS = 100000;

}

Scheduler::Scheduler(std::vector<Process> processes, Config cfg)
    : procs_(std::move(processes)), cfg_(cfg) {
    int ready = 0;
    int blocked = 0;
    for (Process& p : procs_) {
        p.remaining = p.bursts.front();

        const int name = static_cast<int>(p.name.size());
        ready += name + 1;
        int longestIo = 0;
        for (std::size_t i = 1; i < p.bursts.size(); i += 2) longestIo = std::max(longestIo, p.bursts[i]);
        if (longestIo > 0) blocked += name + static_cast<int>(std::to_string(longestIo).size()) + 3;
    }
    cpuWidth_ = static_cast<int>(longestName(procs_, 2)) + 1;
    readyWidth_ = std::max(5, ready - 1);
    blockedWidth_ = std::max(7, blocked - 1);
}

void Scheduler::run() {
    if (cfg_.verbose) printTraceHeader();

    while (!allTerminated()) {
        if (clock_ >= MAX_TICKS) throw std::runtime_error("la simulacion supera el limite de ticks");

        admitArrivals();
        wakeBlocked();
        releaseCpu();
        dispatch();
        if (cfg_.verbose) printTraceLine();
        execute();
        ++clock_;
    }

    if (cfg_.verbose) printTraceLine();
}

void Scheduler::admitArrivals() {
    for (Process& p : procs_) {
        if (p.state == State::NEW && p.arrival == clock_) {
            toReady(&p);
            event(p.name + " llega -> ready");
        }
    }
}

void Scheduler::wakeBlocked() {
    for (auto it = blocked_.begin(); it != blocked_.end();) {
        Process* p = *it;
        if (p->remaining > 0) {
            ++it;
            continue;
        }
        ++p->burst;
        p->remaining = p->bursts[p->burst];
        toReady(p);
        event(p->name + " fin de e/s -> ready");
        it = blocked_.erase(it);
    }
}

void Scheduler::releaseCpu() {
    if (!running_) return;
    Process* p = running_;

    if (cfg_.algorithm == Algorithm::RR && sliceUsed_ >= cfg_.quantum) {
        if (ready_.empty()) {
            sliceUsed_ = 0;
            event(p->name + " agota quantum, cola vacia: sigue");
        } else {
            event(p->name + " agota quantum -> ready");
            toReady(p);
            running_ = nullptr;
        }
        return;
    }

    if (isPreemptive() && !ready_.empty()) {
        const Process* best = *pickNext();
        if (beats(best, p)) {
            event(p->name + " expulsado por " + best->name + " -> ready");
            toReady(p);
            running_ = nullptr;
        }
    }
}

void Scheduler::dispatch() {
    if (running_) return;

    if (incoming_) {
        if (switchLeft_ > 0) return;
        Process* p = incoming_;
        incoming_ = nullptr;
        start(p);
        return;
    }

    if (ready_.empty()) return;

    auto it = pickNext();
    Process* next = *it;
    ready_.erase(it);

    if (lastOnCpu_ && lastOnCpu_ != next) {
        ++contextSwitches_;
        if (cfg_.contextSwitch > 0) {
            incoming_ = next;
            switchLeft_ = cfg_.contextSwitch;
            event("cambio de contexto " + lastOnCpu_->name + " => " + next->name);
            return;
        }
    }
    start(next);
}

void Scheduler::execute() {
    for (Process& p : procs_) {
        char c = ' ';
        if (p.state == State::READY) {
            c = '-';
            ++p.waiting;
        } else if (p.state == State::RUNNING) {
            c = 'r';
        } else if (p.state == State::BLOCKED) {
            c = 'b';
        }
        p.timeline += c;
    }

    for (Process* p : blocked_) --p->remaining;

    if (incoming_) {
        cpuLine_ += '#';
        ++switchTicks_;
        --switchLeft_;
        return;
    }

    if (!running_) {
        cpuLine_ += '.';
        ++idleTicks_;
        return;
    }

    Process* p = running_;
    cpuLine_ += '*';
    ++busyTicks_;
    ++sliceUsed_;
    if (p->firstRun < 0) p->firstRun = clock_;

    if (--p->remaining > 0) return;

    running_ = nullptr;
    ++p->burst;
    if (p->burst == p->bursts.size()) {
        p->state = State::TERMINATED;
        p->finish = clock_ + 1;
        event(p->name + " termina -> terminated");
    } else {
        p->state = State::BLOCKED;
        p->remaining = p->bursts[p->burst];
        blocked_.push_back(p);
        event(p->name + " pide e/s (" + std::to_string(p->remaining) + ") -> blocked");
    }
}

bool Scheduler::allTerminated() const {
    return std::all_of(procs_.begin(), procs_.end(),
                       [](const Process& p) { return p.state == State::TERMINATED; });
}

void Scheduler::toReady(Process* p) {
    p->state = State::READY;
    ready_.push_back(p);
}

void Scheduler::start(Process* p) {
    p->state = State::RUNNING;
    running_ = p;
    lastOnCpu_ = p;
    sliceUsed_ = 0;
    event(p->name + " -> running");
}

void Scheduler::event(const std::string& text) {
    if (cfg_.verbose) events_.push_back(text);
}
