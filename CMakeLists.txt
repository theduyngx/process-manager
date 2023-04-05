cmake_minimum_required(VERSION 3.24)
project(project1_clion C)

set(CMAKE_C_STANDARD 11)

add_executable(project1_clion
        src/main.c
        src/process_pseudo.c
        src/scheduler.c
        src/queue.c
        src/heap.c
        src/mem_alloc.c)

target_include_directories(project1_clion PUBLIC include/)