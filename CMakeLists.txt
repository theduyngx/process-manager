cmake_minimum_required(VERSION 3.24)
project(project1_clion C)

set(CMAKE_C_STANDARD 11)

add_executable(project1_clion main.c sha256.h sha256.c process_pseudo.c scheduling.c sort.c sort.h pseudo_process.h)
