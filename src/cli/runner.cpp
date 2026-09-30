#include "cli/runner.h"

#include <iomanip>
#include <iostream>

#include "sim/scheduler.h"

void simulate(const std::vector<Process>& procs, const Config& cfg) {
    std::cout << "\nalgoritmo:          " << algorithmName(cfg) << "\n"
              << "expulsion:          " << preemptionLabel(cfg) << "\n"
              << "cambio de contexto: " << cfg.contextSwitch << " tick(s)\n";

    Scheduler scheduler(procs, cfg);
    scheduler.run();
    scheduler.printTimeline();
    scheduler.printStats();
}

void compare(const std::vector<Process>& procs, Config base) {
    base.verbose = false;

    std::vector<Config> configs;
    for (Algorithm algorithm : {Algorithm::FCFS, Algorithm::SJF, Algorithm::RR, Algorithm::PRIORITY}) {
        Config cfg = base;
        cfg.algorithm = algorithm;
        cfg.preemptive = false;
        configs.push_back(cfg);
        if (algorithm == Algorithm::SJF || algorithm == Algorithm::PRIORITY) {
            cfg.preemptive = true;
            configs.push_back(cfg);
        }
    }

    std::cout << "\ncomparativa (cambio de contexto: " << base.contextSwitch << " tick(s))\n\n"
              << std::left << std::setw(24) << "algoritmo" << std::right << std::setw(9) << "retorno" << std::setw(8)
              << "espera" << std::setw(11) << "respuesta" << std::setw(10) << "uso cpu" << std::setw(9) << "cambios"
              << std::setw(7) << "total" << "\n";
    for (const Config& cfg : configs) {
        Scheduler scheduler(procs, cfg);
        scheduler.run();
        const Summary s = scheduler.summary();
        std::cout << std::left << std::setw(24) << algorithmName(cfg) << std::right << std::fixed
                  << std::setprecision(2) << std::setw(9) << s.avgTurnaround << std::setw(8) << s.avgWaiting
                  << std::setw(11) << s.avgResponse << std::setprecision(1) << std::setw(9) << s.cpuUsage << "%"
                  << std::setw(9) << s.contextSwitches << std::setw(7) << s.totalTime << "\n";
    }
}
