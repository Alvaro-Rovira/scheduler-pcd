#include "cli/options.h"

#include <iostream>
#include <stdexcept>

#include "cli/parse.h"

void printUsage() {
    std::cout << "uso: scheduler [opciones]\n\n"
              << "  -a, --algo <fcfs|sjf|srtf|rr|prio>  algoritmo de planificacion (por defecto rr)\n"
              << "  -q, --quantum <n>                   quantum de round-robin en ticks (por defecto 3)\n"
              << "  -p, --preempt                       con expulsion (para sjf y prio)\n"
              << "  -c, --cs <n>                        coste del cambio de contexto en ticks (por defecto 1)\n"
              << "  -f, --file <ruta>                   fichero de procesos (si no, carga de ejemplo)\n"
              << "      --compare                       compara todos los algoritmos con la misma carga\n"
              << "      --quiet                         no imprime la traza tick a tick\n"
              << "  -h, --help                          muestra esta ayuda\n\n"
              << "formato del fichero, un proceso por linea (# para comentarios):\n"
              << "  nombre llegada prioridad cpu [es cpu]...\n";
}

Options parseArgs(int argc, char** argv) {
    Options opt;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        auto value = [&]() -> std::string {
            if (i + 1 >= argc) throw std::runtime_error("falta el valor de " + arg);
            return argv[++i];
        };

        if (arg == "-a" || arg == "--algo") {
            const std::string name = value();
            if (name == "fcfs") opt.cfg.algorithm = Algorithm::FCFS;
            else if (name == "sjf") opt.cfg.algorithm = Algorithm::SJF;
            else if (name == "srtf") { opt.cfg.algorithm = Algorithm::SJF; opt.cfg.preemptive = true; }
            else if (name == "rr") opt.cfg.algorithm = Algorithm::RR;
            else if (name == "prio") opt.cfg.algorithm = Algorithm::PRIORITY;
            else throw std::runtime_error("algoritmo desconocido: " + name);
        } else if (arg == "-q" || arg == "--quantum") {
            opt.cfg.quantum = parseInt(value(), "quantum", 1);
        } else if (arg == "-p" || arg == "--preempt") {
            opt.cfg.preemptive = true;
        } else if (arg == "-c" || arg == "--cs") {
            opt.cfg.contextSwitch = parseInt(value(), "cambio de contexto", 0);
        } else if (arg == "-f" || arg == "--file") {
            opt.file = value();
        } else if (arg == "--compare") {
            opt.compare = true;
        } else if (arg == "--quiet") {
            opt.cfg.verbose = false;
        } else if (arg == "-h" || arg == "--help") {
            opt.help = true;
        } else {
            throw std::runtime_error("opcion desconocida: " + arg);
        }
    }
    return opt;
}
