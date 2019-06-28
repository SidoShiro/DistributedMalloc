#include <stdio.h>
#include "utils.h"
#include "cli.h"
#include "graph.h"

int main(int argc, char **argv) {
    // Parse and Work on bin arguments
    (void) argc;
    (void) argv;

    // Start CLI
    //start_cli();

    // Graph
    struct adjency_matrix *a = dot_reader("../simplegraph.dot");
    free_adjency_matrix(a);
    return 0;
}
