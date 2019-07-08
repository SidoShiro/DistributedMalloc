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
    BLOCK_REGISTER,
    LEADER_RES,
    SPLIT_BLOCK,
    NODE,
    ALLOCATION,
    LEADER,
    COMMAND_QUEUE,
    MESSAGE
};

int test_of(char *s, enum test t) {
    char *r = "\033[1;31m";
    char *g = "\033[1;36m";
    char *f = "\033[0m\0";
    printf("=>> Test of : %s\n", s);
    int succ = 0;
    int nb_test = 0;
    switch (t) {
        case BLOCK:
            nb_test = 5;
            struct block *b = generate_block(1, 12, 0, 0, 0);
            if (b->id == 1)
                succ++;
            if (b->size == 12)
                succ++;
            if (b->node_address == 0)
                succ++;
            if (b->virtual_address == 0)
                succ++;
            if (b->next == NULL)
                succ++;

            break;
        case BLOCKS:
            nb_test = 5;
            struct block_register *blks = generate_blocks(2);
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

            struct block *b1 = generate_block(3, 36, 0, 0, 0);
            struct block *b2 = generate_block(4, 36, 36, 0, 0);
            blks->blks[0] = b1;
            blks->blks[1] = b2;

            for (size_t i = 0; i < blks->nb_blocks; i++) {
                if (blks->blks[i] != NULL)
                    succ++;
                else
                    printf("%sERRR: check fail: blks->blks[i] != NULL%s\n", r, f);
            }

            break;
        case BLOCK_REGISTER:
            nb_test = 0;
            struct block_register *b_r = init_nodes_same_size(2, 36);
            nb_test++;
            if (b_r)
                succ++;
            else
                printf("%s>>> fail: b_r != NULL%s\n", r, f);
            nb_test++;
            if (b_r->nb_blocks == 2)
                succ++;
            else
                printf("%s>>> fail: b_r != NULL%s\n", r, f);
            for (size_t i = 0; i < b_r->nb_blocks; i++) {
                nb_test++;
                if (b_r->blks[i] != NULL)
                    succ++;
                else
                    printf("%s>>> fail: b_r->blks[i] != NULL %s\n", r, f);
                nb_test++;
                if (b_r->blks[i]->size == 36)
                    succ++;
                else
                    printf("%s>>> fail: b_r->blks[i]->size == 36  %s\n", r, f);
            }

            break;
        case SPLIT_BLOCK:
            nb_test = 0;
            struct block *bbb1 = generate_block(1, 234, 0, 0, 0);
            struct block *bbb2 = split_block_u(bbb1, 200);
            nb_test++;
            if (bbb1)
                succ++;
            nb_test++;
            if (bbb2)
                succ++;
            nb_test++;
            if (bbb2->size == 200)
                succ++;
            nb_test++;
            if (bbb2->next)
                succ++;
            nb_test++;
            if (bbb2->next && bbb2->next->size == 34)
                succ++;
            else
                printf("%s>>> fail: bbb2->next && bbb2->size == 34 %s\n", r, f);
            break;
        case LEADER_RES:
            nb_test = 0;
            struct node *n = generate_node(1, 345);
            struct leader_resources *l_r = generate_leader_resources(5, 1);

            nb_test++;
            if (l_r->id == n->id)
                succ++;
            else
                printf("%s>>> fail: l_r->id == n->id  %s\n", r, f);

            nb_test++;
            if (l_r->leader_blks != NULL)
                succ++;
            else
                printf("%s>>> fail: l_r->leader_blks != NULL  %s\n", r, f);

            nb_test++;
            if (l_r->leader_command_queue == NULL)
                succ++;
            else
                printf("%s>>> fail: l_r->leader_command_queue != NULL  %s\n", r, f);

            nb_test++;
            if (l_r->leader_reg != NULL)
                succ++;
            else
                printf("%s>>> fail: l_r->leader_reg != NULL  %s\n", r, f);


            break;
        case MESSAGE:
            nb_test = 345;
            for (size_t o = 0; o < 345; o++ ) {
                void *pMessage = generate_message(1, DEF_NODE_USER, DEF_NODE_USER, 0, 0, OP_MALLOC);
                succ++;
                (void) pMessage;
            }
            break;
        case NODE:
            nb_test = 0;
            struct node *no_t = generate_node(1, 256);
            nb_test++;
            if (no_t)
                succ++;
            else {
                printf("%s>>> fail: n != NULL  %s\n", r, f);
                return 0;
            }

            nb_test++;
            if (no_t->id == 1)
                succ++;
            else
                printf("%s>>> fail: n->id == 1  %s\n", r, f);

            nb_test++;
            if (no_t->size == 256)
                succ++;
            else
                printf("%s>>> fail: n->size == 256  %s\n", r, f);

            nb_test++;
            if (no_t->isleader == 0)
                succ++;
            else
                printf("%s>>> fail: n->isleader == 0  %s\n", r, f);


            break;
        default:
            printf("%s>>> INFO : INVALID TEST REQUESTED %s\n", r, f);
            return 0;
    }
    printf("%s=== Tests %s : %d / %d SUCCEEDED =====%s\n\n", g, s, succ, nb_test, f);
    return succ == nb_test ? 1 : 0;
}

int main(int argc, char **argv) {
    (void) argc;
    (void) argv;
    char *r = "\033[1;31m";
    char *g = "\033[1;36m";
    (void) r;
    char *f = "\033[0m\0";
    int nb_test = 0;
    int succ = 0;
    printf("%s===== --- TESTS START --- =====%s\n\n", g, f);

    nb_test++;
    succ += test_of("Block", BLOCK);

    nb_test++;
    succ += test_of("Blocks", BLOCKS);

    nb_test++;
    succ += test_of("Block Register", BLOCK_REGISTER);

    nb_test++;
    succ += test_of("Leader Resources Allocation", LEADER_RES);

    nb_test++;
    succ += test_of("Message Generation", MESSAGE);

    nb_test++;
    succ += test_of("Node", NODE);

    nb_test++;
    succ += test_of("Split Block", SPLIT_BLOCK);


    printf("%s===== %d / %d TESTS SUCCEEDED =====%s\n\n", g, succ, nb_test, f);
}
