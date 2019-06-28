#include "cli.h"
#include "graph.h"
#include "utils.h"

#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv) {
    // Initialize MPI
    int rank, size, len;
    char version[MPI_MAX_LIBRARY_VERSION_STRING];

    // Load Graph Here
    struct adjacency_matrix *a = dot_reader("../simplegraph.dot");
    free_adjacency_matrix(a);

    // Generate Adjacency Matrix Here
    void *adj_mat_network = NULL;
    void *list_id_size = NULL;
    (void) adj_mat_network;
    (void) list_id_size;

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

        // Node Creation
        // Form rank number !

        // Start Leader !


        // Wait Leader

        // If is leader



        // else



    }

    MPI_Finalize();
    return 0;
}
