cmake_minimum_required(VERSION 3.0)

set(CMAKE_C_FLAGS "-O3 -std=c99 -Werror")

project(DistributedMalloc)

# Vars with files for compilation
set(SRCS src/main.c src/utils.c)
set(HEADERS include/utils.h)

add_executable(dmalloc ${SRCS} ${HEADERS})

