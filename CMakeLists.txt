cmake_minimum_required(VERSION 3.0)

set(CMAKE_C_COMPILER "mpicc")
set(CMAKE_C_FLAGS "-O3 -std=c11 -Werror -Wall -Wextra -pedantic -fsanitize=address")

project(DistributedMalloc)

find_package(MPI REQUIRED)

# Vars with files for compilation
set(SRCS 
    src/utils/utils.c
    src/cli/cli.c
    src/network/message.c
    src/network/block.c
    src/network/node.c
    src/utils/command_queue.c
    src/graph/graph.c
    src/graph/map.c
    src/network/leader.c
    src/network/leader_election.c
    src/utils/debug.c
    src/cli/user.c
    src/network/communication.c
    src/network/data_communication.c
    src/utils/queue.c)


set(MAIN src/main/main.c)

set(TEST_MAIN test/main_test.c)

set(FAILURE_DEMO src/main/main_election_with_failures.c)


include_directories("include/")
include_directories(SYSTEM ${MPI_INCLUDE_PATH})


add_executable(dmalloc ${SRCS} ${MAIN} ${MPI_C_LIBRARIES})

add_executable(test_dmalloc ${SRCS} ${TEST_MAIN} ${MPI_C_LIBRARIES})

add_executable(failure_demo ${SRCS} ${FAILURE_DEMO} ${MPI_C_LIBRARIES})

