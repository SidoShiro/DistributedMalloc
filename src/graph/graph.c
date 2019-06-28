#include "utils.h"
#include "graph.h"

void get_values(char str[128], unsigned values[2]);
struct adjency_matrix* generate_adjency_matrix(unsigned* links, unsigned max, unsigned nb_links);
void print_adjency_matrix(struct adjency_matrix* a);


struct adjency_matrix* dot_reader(char* path) {

    FILE *file = fopen(path, "r" );
    unsigned* links = calloc(100, sizeof(unsigned[2]));
    unsigned max = 0;
    unsigned nb_links = 0;
    
    if (file != NULL) {
        char line [ 128 ];
        unsigned i = 0;

        fgets ( line, sizeof line, file );

        while ( fgets ( line, sizeof line, file ) != NULL && line[0] != '}') {
            unsigned val[2] = {0, 0};
            get_values(line, val);

            unsigned tmp_max = val[0] > val[1] ? val[0] : val[1];
            max = max > tmp_max ? max : tmp_max; // The idea is to capture the largest id, for adjency dimensions

            // TODO: Realloc if needed
            links[i] = val[0];
            links[i + 1] = val[1];
            i += 2;
            nb_links += 1;
        }
        fclose ( file );
    }

    struct adjency_matrix *a = generate_adjency_matrix(links, max + 1, nb_links);
    free(links);

    print_adjency_matrix(a);

    return a;
}


void get_values(char str[128], unsigned values[2]) {
    char c;
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

struct adjency_matrix* generate_adjency_matrix(unsigned* links, unsigned dim, unsigned nb_links) { // unused nb_links

    struct adjency_matrix* a = malloc(sizeof(struct adjency_matrix));
    a->matrix = calloc(sizeof(unsigned*), dim);
    a->dimension = dim;
    // +1 cause id0 is for user ?
    for (unsigned i = 0; i < dim; ++i) {
        a->matrix[i] = calloc(sizeof(unsigned*), dim);
    }

    for (unsigned i = 0; links[i]; ++i) {
        a->matrix[links[i]][links[i + 1]] = 1;
        a->matrix[links[i + 1]][links[i]] = 1;
        
        i += 1;
    }

    return a;
}

void free_adjency_matrix(struct adjency_matrix* a) {
    for (unsigned i = 0; i < a->dimension; ++i) {
        free(a->matrix[i]);
    }
    free(a->matrix);
    free(a);
}

void print_adjency_matrix(struct adjency_matrix* a) {
    for (unsigned i = 0; i < a->dimension; ++i) {
        unsigned j = 0;
        for (; j < a->dimension - 1; ++j) {
            printf("%u ", a->matrix[i][j]);
        }
        printf("%u\n", a->matrix[i][j]);
    }
}

