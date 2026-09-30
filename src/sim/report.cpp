#include <iomanip>
#include <iostream>

#include "sim/scheduler.h"

void Scheduler::printTraceHeader() const {
    std::cout << "\ntraza (>px = cambio de contexto hacia px; entre parentesis, e/s restante)\n\n"
              << "   t | " << std::left << std::setw(cpuWidth_) << "cpu" << " | " << std::setw(readyWidth_) << "ready"
              << " | " << std::setw(blockedWidth_) << "blocked" << " | eventos\n"
              << std::right;
    std::cout << "-----+-" << std::string(cpuWidth_, '-') << "-+-" << std::string(readyWidth_, '-') << "-+-"
              << std::string(blockedWidth_, '-') << "-+--------\n";
}

void Scheduler::printTraceLine() {
    std::string cpu = "-";
    if (running_) cpu = running_->name;
    else if (incoming_) cpu = ">" + incoming_->name;

    std::string ready;
    for (const Process* p : ready_) ready += (ready.empty() ? "" : " ") + p->name;
    if (ready.empty()) ready = "-";

    std::string blocked;
    for (const Process* p : blocked_) {
        blocked += (blocked.empty() ? "" : " ") + p->name + "(" + std::to_string(p->remaining) + ")";
    }
    if (blocked.empty()) blocked = "-";

    std::string events;
    for (const std::string& e : events_) events += (events.empty() ? "" : "; ") + e;
    events_.clear();

    std::cout << std::setw(4) << clock_ << " | " << std::left << std::setw(cpuWidth_) << cpu << " | "
              << std::setw(readyWidth_) << ready << " | " << std::setw(blockedWidth_) << blocked << " | " << std::right
              << events << "\n";
}

void Scheduler::printTimeline() const {
    const int w = static_cast<int>(longestName(procs_, 3));

    std::string ruler(static_cast<std::size_t>(clock_) + 1, ' ');
    for (int t = 0; t <= clock_; t += 5) {
        const std::string n = std::to_string(t);
        const std::size_t pos = static_cast<std::size_t>(t);
        if (ruler.size() < pos + n.size()) ruler.resize(pos + n.size(), ' ');
        ruler.replace(pos, n.size(), n);
    }

    std::cout << "\ncronograma (1 columna = 1 tick)\n\n" << std::left;
    std::cout << std::setw(w) << "t" << "  " << ruler << "\n";
    for (const Process& p : procs_) std::cout << std::setw(w) << p.name << "  " << p.timeline << "\n";
    std::cout << std::setw(w) << "cpu" << "  " << cpuLine_ << "\n" << std::right;
    std::cout << "\n  procesos: r = running   - = ready   b = blocked (e/s)\n"
              << "  cpu:      * = ocupada   # = cambio de contexto   . = libre\n";
}

void Scheduler::printStats() const {
    const int w = static_cast<int>(longestName(procs_, 7));
    const Summary s = summary();

    std::cout << "\nestadisticas\n\n";
    std::cout << std::left << std::setw(w) << "proceso" << std::right << std::setw(9) << "llegada" << std::setw(6)
              << "cpu" << std::setw(6) << "e/s" << std::setw(6) << "fin" << std::setw(9) << "retorno" << std::setw(8)
              << "espera" << std::setw(11) << "respuesta" << "\n";
    for (const Process& p : procs_) {
        std::cout << std::left << std::setw(w) << p.name << std::right << std::setw(9) << p.arrival << std::setw(6)
                  << p.total(true) << std::setw(6) << p.total(false) << std::setw(6) << p.finish << std::setw(9)
                  << p.finish - p.arrival << std::setw(8) << p.waiting << std::setw(11) << p.firstRun - p.arrival
                  << "\n";
    }
    std::cout << std::fixed << std::setprecision(2) << std::left << std::setw(w) << "media" << std::right
              << std::setw(27) << "" << std::setw(9) << s.avgTurnaround << std::setw(8) << s.avgWaiting
              << std::setw(11) << s.avgResponse << "\n";

    std::cout << "\n  tiempo total:        " << s.totalTime << " ticks\n"
              << "  uso de cpu:          " << std::setprecision(1) << s.cpuUsage << "% (" << busyTicks_
              << " ejecutando, " << switchTicks_ << " en cambio de contexto, " << idleTicks_ << " libre)\n"
              << "  cambios de contexto: " << s.contextSwitches << "\n";
}

Summary Scheduler::summary() const {
    Summary s;
    for (const Process& p : procs_) {
        s.avgTurnaround += p.finish - p.arrival;
        s.avgWaiting += p.waiting;
        s.avgResponse += p.firstRun - p.arrival;
    }
    const double n = static_cast<double>(procs_.size());
    s.avgTurnaround /= n;
    s.avgWaiting /= n;
    s.avgResponse /= n;
    s.totalTime = clock_;
    s.cpuUsage = clock_ > 0 ? 100.0 * busyTicks_ / clock_ : 0.0;
    s.contextSwitches = contextSwitches_;
    return s;
}
