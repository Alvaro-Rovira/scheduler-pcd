cmake_minimum_required(VERSION 3.16)
project(scheduler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(scheduler
        src/main.cpp
        src/cli/options.cpp
        src/cli/parse.cpp
        src/cli/runner.cpp
        src/cli/workload.cpp
        src/sim/config.cpp
        src/sim/policy.cpp
        src/sim/report.cpp
        src/sim/scheduler.cpp
)

target_include_directories(scheduler PRIVATE src)

if (MSVC)
    target_compile_options(scheduler PRIVATE /W4)
else ()
    target_compile_options(scheduler PRIVATE -Wall -Wextra -Wpedantic)
endif ()
