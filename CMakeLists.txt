cmake_minimum_required(VERSION 3.0)

set(CMAKE_C_FLAGS "-O3 -std=c11 -Werror")

project(DistributedMalloc)

# Vars with files for compilation
set(SRCS src/main.c src/utils.c src/cli.c)
set(HEADERS include/utils.h include/cli.h)

add_executable(dmalloc ${SRCS} ${HEADERS})

