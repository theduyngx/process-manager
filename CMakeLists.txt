cmake_minimum_required(VERSION 3.24)
project(project1_clion C)

set(CMAKE_C_STANDARD 11)

add_executable(project1_clion main.c process_pseudo.c scheduler.c sort.c input_queue.c ready_queue.c)

target_include_directories(project1_clion PUBLIC includes/)
