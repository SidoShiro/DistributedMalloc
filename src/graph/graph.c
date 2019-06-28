#include "utils.h"
#include "graph.h"

void get_values(char str[128], unsigned values[2]);
struct adjacency_matrix* generate_adjacency_matrix(unsigned* links, unsigned max, unsigned nb_links);
void print_adjacency_matrix(struct adjacency_matrix* a);


struct adjacency_matrix* dot_reader(char* path) {

    FILE *file = fopen(path, "r" );
    size_t nb_links_possible = 300;
    unsigned* links = calloc(nb_links_possible, sizeof(unsigned[2]));
    unsigned max = 0;
    unsigned nb_links = 0;
    
    if (file != NULL) {

        char line [ 128 ];
        unsigned i = 0;

        void *trash = fgets(line, sizeof line, file);
        trash = trash; // boring flags

        while (fgets(line, sizeof line, file) != NULL && line[0] != '}') {
            unsigned val[2] = {0, 0};
            get_values(line, val);

            unsigned tmp_max = val[0] > val[1] ? val[0] : val[1];
            max = max > tmp_max ? max : tmp_max; // The idea is to capture the largest id, for adjacency dimensions

            if (nb_links == nb_links_possible) {
                nb_links_possible *= 2;
                links = realloc(links, nb_links_possible * sizeof(unsigned[2]));
            }

            links[i] = val[0];
            links[i + 1] = val[1];
            i += 2;
            nb_links += 1;
        }
        fclose ( file );
    }

    struct adjacency_matrix *a = generate_adjacency_matrix(links, max + 1, nb_links);
    free(links);

    print_adjacency_matrix(a);

    return a;
}


void get_values(char str[128], unsigned values[2]) {
    unsigned i = 0;

    // spaces
    while(str[i] == ' ')
        ++i;
    // first val
    while(str[i] != ' ') {
        values[0] = values[0] * 10 + str[i] - 48;
        ++i;
    }
    // sep
    i += 4;
    // second val

    while(str[i] != ';') {
        values[1] = values[1] * 10 + (unsigned) str[i] - 48;
        ++i;
    }
}

struct adjacency_matrix* generate_adjacency_matrix(unsigned* links, unsigned dim, unsigned nb_links) { // unused nb_links

    struct adjacency_matrix* a = malloc(sizeof(struct adjacency_matrix));
    a->matrix = calloc(sizeof(unsigned*), dim);
    a->dimension = dim;
    // +1 cause id0 is for user ?
    for (unsigned i = 0; i < dim; ++i) {
        a->matrix[i] = calloc(sizeof(unsigned*), dim);
    }

    for (unsigned i = 0; i < nb_links; ++i) {
        a->matrix[links[2 * i]][links[2 * i + 1]] = 1;
        a->matrix[links[2 * i + 1]][links[2 * i]] = 1;
        
        i += 1;
    }

    

    return a;
}

void free_adjacency_matrix(struct adjacency_matrix* a) {
    for (unsigned i = 0; i < a->dimension; ++i) {
        free(a->matrix[i]);
    }
    free(a->matrix);
    free(a);
}

void print_adjacency_matrix(struct adjacency_matrix* a) {
    for (unsigned i = 0; i < a->dimension; ++i) {
        unsigned j = 0;
        for (; j < a->dimension - 1; ++j) {
            printf("%u ", a->matrix[i][j]);
        }
        printf("%u\n", a->matrix[i][j]);
    }
}

