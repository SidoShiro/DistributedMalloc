#include "cli.h"
#include "graph.h"
#include "utils.h"
#include "node.h"
#include "leader.h"
#include "globals.h"
#include "debug.h"
#include "../include/block.h"
#include "leader_election.h"

#include <stdio.h>
#include <stdlib.h>

enum test {
    BLOCK,
    BLOCKS,
    ALLOCATION,
    LEADER,
    COMMAND_QUEUE,
    MESSAGE
};

int test_of(char *s, enum test t) {
    char *r = "\033[1;31m";
    char *g = "\033[1;36m";
    char *f = "\033[0m\0";
    printf("=> Test of : %s\n", s);
    int succ = 0;
    int nb_test = 0;
    switch (t) {
        case BLOCK:
            nb_test = 4;
            struct block *b = generate_block(1, 12, 0);
            if (b->id == 1)
                succ++;
            if (b->size == 12)
                succ++;
            if (b->address == 0)
                succ++;
            if (b->next == NULL)
                succ++;
            printf("%s=== Tests %s : %d / %d SUCCEEDED =====%s\n\n", g, s, succ, nb_test, f);
            break;
        case BLOCKS:
            nb_test = 5;
            struct blocks *blks = generate_blocks(2);
            if (blks->nb_blocks == 2)
                succ++;
            else
                printf("%sERRR: check fail: blks->nb_blocks == 2%s\n", r, f);
            for (size_t i = 0; i < blks->nb_blocks; i++) {
                if (blks->blks[i] == NULL)
                    succ++;
                else
                    printf("%sERRR: check fail: blks->blks[i] == NULL%s\n", r, f);
            }

            struct block *b1 = generate_block(3, 36, 0);
            struct block *b2 = generate_block(4, 36, 36);
            blks->blks[0] = b1;
            blks->blks[1] = b2;

            for (size_t i = 0; i < blks->nb_blocks; i++) {
                if (blks->blks[i] != NULL)
                    succ++;
                else
                    printf("%sERRR: check fail: blks->blks[i] != NULL%s\n", r, f);
            }

            printf("%s=== Tests %s : %d / %d SUCCEEDED =====%s\n\n", g, s, succ, nb_test, f);
            break;
        case ALLOCATION:

            printf("%s=== Tests %s : %d / %d SUCCEEDED =====%s\n\n", g, s, succ, nb_test, f);
            break;
        case LEADER:

            printf("%s=== Tests %s : %d / %d SUCCEEDED =====%s\n\n", g, s, succ, nb_test, f);
            break;
        case COMMAND_QUEUE:

            printf("%s=== Tests %s : %d / %d SUCCEEDED =====%s\n\n", g, s, succ, nb_test, f);
            break;
        case MESSAGE:
            printf("%s=== Tests %s : %d / %d SUCCEEDED =====%s\n\n", g, s, succ, nb_test, f);
            break;
    }
    return 1;
}

int main(int argc, char **argv) {
    (void) argc;
    (void) argv;
    char *r = "\033[1;31m";
    char *g = "\033[1;36m";
    (void) r;
    char *f = "\033[0m\0";
    int nb_test = 2;
    int succ = 0;
    // BLOCK
    succ += test_of("Block", BLOCK);

    succ += test_of("Block", BLOCKS);

    printf("%s===== %d / %d TESTS SUCCEEDED =====%s\n\n", g, succ, nb_test, f);
}
