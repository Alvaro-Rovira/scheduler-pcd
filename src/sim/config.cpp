#include "sim/config.h"

std::string algorithmName(const Config& cfg) {
    switch (cfg.algorithm) {
        case Algorithm::FCFS: return "fcfs";
        case Algorithm::SJF: return cfg.preemptive ? "sjf expulsivo (srtf)" : "sjf";
        case Algorithm::RR: return "round-robin (q=" + std::to_string(cfg.quantum) + ")";
        case Algorithm::PRIORITY: return cfg.preemptive ? "prioridades expulsivo" : "prioridades";
    }
    return "?";
}

std::string preemptionLabel(const Config& cfg) {
    switch (cfg.algorithm) {
        case Algorithm::FCFS: return "no";
        case Algorithm::RR: return "si, al agotar el quantum";
        case Algorithm::SJF: return cfg.preemptive ? "si, si llega una rafaga mas corta" : "no";
        case Algorithm::PRIORITY: return cfg.preemptive ? "si, si llega uno mas prioritario" : "no";
    }
    return "?";
}
