#include "cli/workload.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "cli/parse.h"

namespace {

Process makeProcess(int pid, std::string name, int arrival, int priority, std::vector<int> bursts) {
    Process p;
    p.pid = pid;
    p.name = std::move(name);
    p.arrival = arrival;
    p.priority = priority;
    p.bursts = std::move(bursts);
    return p;
}

}

std::vector<Process> defaultProcesses() {
    return {
        makeProcess(1, "p1", 0, 3, {5, 3, 2}),
        makeProcess(2, "p2", 1, 1, {3}),
        makeProcess(3, "p3", 2, 4, {6, 2, 3}),
        makeProcess(4, "p4", 4, 2, {2, 4, 2}),
    };
}

std::vector<Process> loadProcesses(const std::string& path) {
    std::ifstream in(path);
    if (!in) throw std::runtime_error("no se puede abrir " + path);

    std::vector<Process> procs;
    std::string line;
    for (int lineNo = 1; std::getline(in, line); ++lineNo) {
        line = line.substr(0, line.find('#'));
        std::istringstream fields(line);
        std::vector<std::string> tokens;
        for (std::string token; fields >> token;) tokens.push_back(token);
        if (tokens.empty()) continue;

        const std::string where = path + ":" + std::to_string(lineNo);
        if (tokens.size() < 4) throw std::runtime_error(where + ": se espera 'nombre llegada prioridad cpu [es cpu]...'");
        if (tokens.size() % 2 != 0) throw std::runtime_error(where + ": las rafagas tienen que empezar y acabar en cpu");

        std::vector<int> bursts;
        for (std::size_t i = 3; i < tokens.size(); ++i) bursts.push_back(parseInt(tokens[i], where + ": rafaga", 1));
        procs.push_back(makeProcess(static_cast<int>(procs.size()) + 1, tokens[0],
                                    parseInt(tokens[1], where + ": llegada", 0),
                                    parseInt(tokens[2], where + ": prioridad", 0), std::move(bursts)));
    }
    if (procs.empty()) throw std::runtime_error(path + ": no contiene ningun proceso");
    return procs;
}

void printWorkload(const std::vector<Process>& procs) {
    std::cout << "\nprocesos\n\n"
              << std::left << std::setw(5) << "pid" << std::setw(10) << "nombre" << std::setw(9) << "llegada"
              << std::setw(11) << "prioridad" << "rafagas\n";
    for (const Process& p : procs) {
        std::cout << std::setw(5) << p.pid << std::setw(10) << p.name << std::setw(9) << p.arrival << std::setw(11)
                  << p.priority;
        for (std::size_t i = 0; i < p.bursts.size(); ++i) {
            std::cout << (i == 0 ? "" : " -> ") << (i % 2 == 0 ? "cpu " : "e/s ") << p.bursts[i];
        }
        std::cout << "\n";
    }
    std::cout << std::right;
}
