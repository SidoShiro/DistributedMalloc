cmake_minimum_required(VERSION 3.0)

set(CMAKE_C_COMPILER "mpicc")
set(CMAKE_C_FLAGS "-O3 -std=c11 -Werror -Wall -Wextra -pedantic")

project(DistributedMalloc)

find_package(MPI REQUIRED)

# Vars with files for compilation
set(SRCS src/main/main.c src/utils/utils.c src/cli/cli.c src/network/message.c src/network/block.c include/block.h src/network/node.c)
include_directories("include/")
include_directories(SYSTEM ${MPI_INCLUDE_PATH})

add_executable(dmalloc ${SRCS} ${MPI_C_LIBRARIES})

