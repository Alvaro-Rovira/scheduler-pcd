#include <exception>
#include <iostream>
#include <vector>

#include "cli/options.h"
#include "cli/runner.h"
#include "cli/workload.h"

int main(int argc, char** argv) {
    try {
        const Options opt = parseArgs(argc, argv);
        if (opt.help) {
            printUsage();
            return 0;
        }

        const std::vector<Process> procs = opt.file.empty() ? defaultProcesses() : loadProcesses(opt.file);

        std::cout << "=== simulador de planificacion de procesos ===\n";
        printWorkload(procs);
        if (opt.compare) compare(procs, opt.cfg);
        else simulate(procs, opt.cfg);
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << "\n(usa --help para ver las opciones)\n";
        return 1;
    }
}
