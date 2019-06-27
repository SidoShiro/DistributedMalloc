#include <stdio.h>
#include "cli.h"

char *read_cmd() {
    ssize_t buffer_size = 256;
    int position = 0;
    char *buffer = malloc(sizeof(char) * buffer_size);
    int c;

    if (!buffer) {
        exit(3);
    }

    while (1) {
        // Read a character
        c = getchar();
        // If we hit EOF, replace it with a null character and return.
        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }
        position++;

        // If we have exceeded the buffer, reallocate.
        if (position >= buffer_size) {
            buffer_size *= 2;
            buffer = realloc(buffer, buffer_size);
            if (!buffer) {
                exit(3);
            }
        }
    }
}


char **split_cmd(char *cmd) {
    char **tokens = NULL;
    char *token;
    ssize_t nb_tok = 1;
    token = strtok(cmd, " ");
    /* walk through other tokens */
    while (token != NULL) {
        tokens = realloc(tokens, nb_tok * sizeof(char *));
        tokens[nb_tok - 1] = token;
        nb_tok++;
        token = strtok(NULL, " ");
    }
    tokens = realloc(tokens, nb_tok * sizeof(char *));
    tokens[nb_tok - 1] = NULL;
    return tokens;
}

void print_args(char **args) {
    char *tok;
    ssize_t i = 0;
    do {
        tok = args[i];
        i++;
        if (tok != NULL)
            printf("%s\n", tok);
    } while (tok != NULL);
}

void error_msg(char *msg) {
    warnx("%s\n", msg);
}

void execute(char **args) {
    size_t l = len(args);
    if (l == 0)
        return;
    if (0 == strcmp(args[0], "h")) {
        printf("dmalloc commands:\n"
               " h                             | display all available commands with their description   |\n"
               " m `size`                      | return `address` to cmd user of the required allocation |\n"
               " f `address`                   | free address, Warning if already free                   |\n"
               " w `address` `datasize` `data` | write at the address the data of size datasize          |\n"
               " r `address` `datasize`        | read datasize bytes at address                          |\n"
               " d `address`                   | dump in as text all data of the block stored in address |\n"
               " d `address` `file`            | dump address data in file                               |\n"
               " snap                          | give user a snap of the network (print)                 |\n"
               " snap `file`                   | same as snap but stored in file                         |\n"
               " dnet                          | write a .dot file of the network                        |\n"
               " kill `id`                     | kill node refered by `id`                               |\n"
               " revive `id`                   | revive `id`                                             |\n"
               "\n");
    } else if (0 == strcmp(args[0], "m")) {
        // ERRORS
        if (l == 1) {
            error_msg("m require an argument 'size' (integer)");
            return;
        } else if (l >= 3) {
            error_msg("m do not support more than 1 argument, check command h");
            return;
        }

        // Execution
        size_t size = 0;
        if (1 == sscanf(args[1], "%zu", &size)) {
            printf("Execute DMALLOC of %zu\n", size);
        } else {
            error_msg("m  require an argument 'size' which can be casted as a positive integer");
        }
    } else if (0 == strcmp(args[0], "f")) {
        // ERRORS
        if (l == 1) {
            error_msg("f require an argument 'address'");
            return;
        } else if (l >= 3) {
            error_msg("f do not support more than 1 argument, check command h");
            return;
        }

        // Execution
        size_t address = 0;
        if (1 == sscanf(args[1], "%zu", &address)) {
            printf("Execute Free of address : %zu\n", address);
        } else {
            error_msg("f  require an argument 'address' which can be casted as a positive integer");
        }
    } else if (0 == strcmp(args[0], "w")) {
        // ERRORS
        if (l <= 3) {
            error_msg("w require 3 arguments : 'address', 'datasize' and 'data'");
            return;
        } else if (l >= 5) {
            error_msg("w do not support more than 3 arguments, check command h");
            return;
        }

        // Execution
        size_t address = 0;
        size_t datasize = 0;
        if (1 == sscanf(args[1], "%zu", &address)) {
            if (1 == sscanf(args[2], "%zu", &datasize)) {
                printf("Execute Write at %zu of %s : %zu bytes\n", address, args[3], datasize);
            } else {
                error_msg("w requires an argument 'datasize' which can be casted as a positive integer");
            }
        } else {
            error_msg("w requires an argument 'address' which can be casted as a positive integer");
        }
    } else if (0 == strcmp(args[0], "r")) {
        // ERRORS
        if (l <= 2) {
            error_msg("r requires 2 arguments : 'address' and 'datasize'");
            return;
        } else if (l >= 4) {
            error_msg("r do not support more than 2 arguments, check command h");
            return;
        }

        // Execution
        size_t address = 0;
        size_t datasize = 0;
        if (1 == sscanf(args[1], "%zu", &address)) {
            if (1 == sscanf(args[2], "%zu", &datasize)) {
                printf("Execute Read at %zu, %zu bytes\n", address, datasize);
            } else {
                error_msg("r requires an argument 'datasize' which can be casted as a positive integer");
            }
        } else {
            error_msg("r require an argument 'address' which can be casted as a positive integer");
        }
    } else if (0 == strcmp(args[0], "d")) {
        // ERRORS
        if (l <= 1) {
            error_msg("d requires minimum 1 argument and maximum 2 : 'address' and 'file'");
            return;
        } else if (l >= 4) {
            error_msg("d do not support more than 2 arguments, check command h");
            return;
        }

        // Execution
        size_t address = 0;
        if (1 == sscanf(args[1], "%zu", &address)) {

            // d address
            if (l == 2) {
                printf("Execute Dump of %zu\n", address);

            }

            // d address file
            if (l == 3) {
                printf("Execute Dump of %zu  and stored it in %s\n", address, args[2]);
            }

        } else {
            error_msg("w require an argument 'address' which can be casted as a positive integer");
        }
    } else if (0 == strcmp(args[0], "snap")) {
        // ERRORS
        if (l <= 3) {
            error_msg("w require 3 arguments : 'address', 'datasize' and 'data'");
            return;
        } else if (l >= 5) {
            error_msg("w do not support more than 3 arguments, check command h");
            return;
        }

        // Execution
        size_t address = 0;
        size_t datasize = 0;
        if (1 == sscanf(args[1], "%zu", &address)) {
            if (1 == sscanf(args[2], "%zu", &datasize)) {
                printf("Execute Write at %zu of %s : %zu bytes\n", address, args[3], datasize);
            } else {
                error_msg("w require an argument 'datasize' which can be casted as a positive integer");
            }
        } else {
            error_msg("w require an argument 'address' which can be casted as a positive integer");
        }
    } else if (0 == strcmp(args[0], "w")) {
        // ERRORS
        if (l <= 3) {
            error_msg("w require 3 arguments : 'address', 'datasize' and 'data'");
            return;
        } else if (l >= 5) {
            error_msg("w do not support more than 3 arguments, check command h");
            return;
        }

        // Execution
        size_t address = 0;
        size_t datasize = 0;
        if (1 == sscanf(args[1], "%zu", &address)) {
            if (1 == sscanf(args[2], "%zu", &datasize)) {
                printf("Execute Write at %zu of %s : %zu bytes\n", address, args[3], datasize);
            } else {
                error_msg("w require an argument 'datasize' which can be casted as a positive integer");
            }
        } else {
            error_msg("w require an argument 'address' which can be casted as a positive integer");
        }
    } else if (0 == strcmp(args[0], "w")) {
        // ERRORS
        if (l <= 3) {
            error_msg("w require 3 arguments : 'address', 'datasize' and 'data'");
            return;
        } else if (l >= 5) {
            error_msg("w do not support more than 3 arguments, check command h");
            return;
        }

        // Execution
        size_t address = 0;
        size_t datasize = 0;
        if (1 == sscanf(args[1], "%zu", &address)) {
            if (1 == sscanf(args[2], "%zu", &datasize)) {
                printf("Execute Write at %zu of %s : %zu bytes\n", address, args[3], datasize);
            } else {
                error_msg("w require an argument 'datasize' which can be casted as a positive integer");
            }
        } else {
            error_msg("w require an argument 'address' which can be casted as a positive integer");
        }
    }

}

void start_cli() {
    int no_quit = 1;
    char *cmd;
    char **args;

    while (no_quit) {
        printf("dmalloc $ ");
        fflush(0);
        cmd = read_cmd();

        // DEBUG printf("%s\n", cmd);

        args = split_cmd(cmd);
        if (args && args[0] && 0 == strcmp(args[0], "exit")) {
            no_quit = 0;
            continue;
        }

        // DEBUG print_args(args);

        execute(args);

        free(cmd);
        free(args);
    }
}
