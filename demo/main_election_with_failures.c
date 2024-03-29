#include "cli.h"
#include "graph.h"
#include "utils.h"
#include "node.h"
#include "leader.h"
#include "globals.h"
#include "debug.h"
#include "leader_election.h"

#include <mpi.h>
#include <stdio.h>
#include <queue.h>

int main(int argc, char **argv) {
    // Initialize MPI
    int rank, size, len;
    char version[MPI_MAX_LIBRARY_VERSION_STRING];

    // Load Graph Here
    //struct adjacency_matrix *a = dot_reader("../simplegraph.dot");
    //free_adjacency_matrix(a);

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
    if (rank == DEF_NODE_USER) {
        printf("starting %d processes\n", size);
        debug("Start User", DEF_NODE_USER);
        start_cli(size);
    }
    else
    {
        debug("Start Node", rank);

        struct node *n = generate_node(rank, DEF_NODE_SIZE);
        int new_leader = 0;
        do {
            // HERE WE KILL THE NODE 2
            if (rank == 2)
                break;

            n->isleader = 0;
            node_cycle(n);
            new_leader = leader_election(n->id, size);
            printf("NEW LEADER = %d\n", new_leader);
            if (new_leader == n->id) {

                // HERE WE KILL THE NEW LEADER (only the first one)
                if (new_leader == 1)
                    break;


                n->isleader = 1;
                leader_loop(n, DEF_NODE_USER, size - 1);
            }
        } while (new_leader);
    }

    MPI_Finalize();
    return 0;
}
