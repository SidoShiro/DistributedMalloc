#include "cli.h"
#include "utils.h"

#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv) {
    // Initialize MPI
    int rank, size, len;
    char version[MPI_MAX_LIBRARY_VERSION_STRING];
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Get_library_version(version, &len);

    // TODO: Parse and Work on bin arguments

    // Start CLI
    if (rank == 0) {
        printf("starting %d processes\n", size);
        start_cli();
    }
    else
    {
        // TODO: Je sais pas je suis senser faire quoi
    }

    MPI_Finalize();
    return 0;
}
