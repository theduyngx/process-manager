cmake_minimum_required(VERSION 3.24)
project(project1_clion C)

set(CMAKE_C_STANDARD 11)

add_executable(project1_clion main.c process_pseudo.c scheduler.c sort.c sort.h
        pseudo_process.h input_queue.c input_queue.h)
