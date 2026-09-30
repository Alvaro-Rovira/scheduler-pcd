# scheduler sencillo

simulador por consola de un planificador de cpu (un solo procesador) en c++17 con cmake.
avanza tick a tick y muestra quien tiene la cpu, la cola de ready, los bloqueados y los cambios de estado.

## compilar y ejecutar

    cmake -S . -B build
    cmake --build build
    ./build/scheduler

sin argumentos ejecuta round-robin (quantum 3, cambio de contexto de 1 tick) con una carga de ejemplo.

## opciones

    -a, --algo <fcfs|sjf|srtf|rr|prio>   algoritmo (por defecto rr)
    -q, --quantum <n>                    quantum de round-robin (por defecto 3)
    -p, --preempt                        con expulsion (sjf y prio)
    -c, --cs <n>                         coste del cambio de contexto en ticks (por defecto 1, 0 = gratis)
    -f, --file <ruta>                    fichero de procesos
        --compare                        compara todos los algoritmos con la misma carga
        --quiet                          sin la traza tick a tick

ejemplos:

    ./build/scheduler -a rr -q 2
    ./build/scheduler -a srtf -c 0
    ./build/scheduler -a prio -p -f procesos.txt
    ./build/scheduler --compare -f procesos.txt

## fichero de procesos

un proceso por linea, `#` para comentarios:

    nombre llegada prioridad cpu [es cpu]...

las rafagas alternan cpu y e/s empezando y acabando en cpu. menor numero de prioridad = mas prioritario.
hay un ejemplo en `procesos.txt`.

## que se simula

- estados: new, ready, running, blocked y terminated. un proceso se bloquea al acabar una rafaga de cpu que va seguida de e/s, y la e/s de todos los bloqueados avanza en paralelo.
- quantum: en round-robin un proceso ejecuta como mucho `quantum` ticks seguidos y vuelve al final de la cola. si la cola esta vacia sigue el mismo con un quantum nuevo.
- preemption: fcfs nunca quita la cpu, round-robin la quita al agotar el quantum, sjf y prioridades solo expulsan con `--preempt`.
- cambio de contexto: cuando entra un proceso distinto del ultimo que uso la cpu, la cpu pierde `--cs` ticks sin ejecutar a nadie. se cuentan los cambios y el tiempo perdido.
- orden dentro de un tick: llegadas, fin de e/s, quantum o expulsion, despacho y ejecucion. los empates se resuelven por orden de llegada a la cola.

## salida

- traza tick a tick con cpu, ready, blocked y eventos.
- cronograma por proceso (`r` ejecutando, `-` en ready, `b` bloqueado) y fila de cpu (`*` ocupada, `#` cambio de contexto, `.` libre).
- estadisticas: retorno, espera, respuesta, uso de cpu y cambios de contexto.

## estructura

    CMakeLists.txt
    procesos.txt
    src/
      main.cpp
      sim/
        process.h       proceso y estados
        config.h        configuracion y resumen
        config.cpp      nombres de algoritmos
        scheduler.h     clase scheduler
        scheduler.cpp   bucle y fases de cada tick
        policy.cpp      eleccion del siguiente proceso y expulsion
        report.cpp      traza, cronograma y estadisticas
      cli/
        parse.h/.cpp      lectura de numeros
        options.h/.cpp    argumentos de la linea de comandos
        workload.h/.cpp   carga de procesos y fichero de entrada
        runner.h/.cpp     ejecucion y comparativa
