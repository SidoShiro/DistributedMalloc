#include <stdio.h>
#include <command_queue.h>
#include <mpi.h>
#include <globals.h>
#include <debug.h>
#include <communication.h>
#include <sys/stat.h>
#include "cli.h"
#include "communication.h"

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
    int w = 0;
    ssize_t nb_tok = 1;
    token = strtok(cmd, " ");
    if (token && 0 == strcmp(token, "w"))
        w++;
    /* walk through other tokens */
    while (token != NULL) {
        tokens = realloc(tokens, nb_tok * sizeof(char *));
        tokens[nb_tok - 1] = token;
        nb_tok++;
        if (nb_tok == 4 && w == 1)
            token = strtok(NULL, "\0");
        else
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

void execute(char **args, unsigned short leader) {
    size_t l = len(args);
    if (l == 0)
        return;
    if (0 == strcmp(args[0], "h")) {
        printf("dmalloc commands:\n"
               " h                             | display all available commands with their description   |\n"
               " t                             | show table of allocations                               |\n"
               " m `size`                      | return `address` to cmd user of the required allocation |\n"
               " f `address`                   | free address, Warning if already free                   |\n"
               " w `address` `datasize` `data` | write at the address the data of size datasize          |\n"
               " w `address` `file`            | write all content of file at address                    |\n"
               " w `address` `file` `datasize` | write datasize bytes from file to the address           |\n"
               " r `address` `datasize`        | read datasize bytes at address                          |\n"
               " r `address` `file`            | read all bytes of the block at address into file        |\n"
               " r `address` `file` `datasize` | read datasize bytes at address into file                |\n"
               " d `address`                   | dump in as text all data of the block stored in address |\n"
               " d net                         | dump all allocation                                     |\n"
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
            struct data_size *d_s = generate_data_size(size);
            send_command(OP_MALLOC, d_s, leader);
            free(d_s);
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
            struct data_address *d_a = generate_data_address(address);
            send_command(OP_FREE, d_a, leader);
            free(d_a);
        } else {
            error_msg("f  require an argument 'address' which can be casted as a positive integer");
        }
    } else if (0 == strcmp(args[0], "w")) {
        // ERRORS
        if (l <= 2) {
            error_msg(
                    "w require 2-3 arguments : 'address', 'datasize' and 'data'\n OR 'address' 'file' + 'datasize' optional, check h");
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
                if (args[3] == NULL) {
                    error_msg("w cannot write nothing");
                    return;
                }
                printf("Execute Write at %zu of %s : %zu bytes\n", address, args[3], datasize);
                struct data_write *d_w = generate_data_write(address, datasize, args[3]);
                send_command(OP_WRITE, d_w, leader);
                free(d_w);
            } else {
                // Write from File
                FILE *file_write = fopen(args[2], "r");
                if (!file_write) {
                    error_msg("w 'address' 'file', file was not possible to open, or doesn't exist");
                    return;
                }
                struct stat st;
                /*get the size using stat()*/
                if (stat(args[2], &st) != 0)
                    return;
                if (l == 4) {
                    // file max data read
                    if (1 == sscanf(args[3], "%zu", &datasize)) {
                        if ((ssize_t) datasize >= st.st_size)
                            datasize = st.st_size;
                    } else {
                        error_msg("w requires an argument 'datasize' which can be casted as a positive integer");
                        return;
                    }
                } else {
                    datasize = st.st_size;
                }
                // Now read datasize
                char *buffer_write_file = malloc(sizeof(char) * (2 + datasize));
                size_t r_c = fread(buffer_write_file, sizeof(char), datasize, file_write);
                if (r_c != datasize)
                    error_msg("WARNING: file bytes read different from bytes asked");
                struct data_write *d_w = generate_data_write(address, datasize, buffer_write_file);
                send_command(OP_WRITE, d_w, leader);
                // OLD error_msg("w requires an argument 'datasize' which can be casted as a positive integer");
            }
        } else {
            error_msg("w requires an argument 'address' which can be casted as a positive integer");
        }
    } else if (0 == strcmp(args[0], "r")) {
        // ERRORS
        if (l <= 2) {
            error_msg("r requires 2-3 arguments : 'address' and 'datasize'\n OR 'address' 'file'\n OR 'address' 'file' 'datasize");
            return;
        } else if (l >= 5) {
            error_msg("r do not support more than 2-3 arguments, check command h");
            return;
        }

        // Execution
        size_t address = 0;
        size_t datasize = 0;
        if (1 == sscanf(args[1], "%zu", &address)) {
            if (1 == sscanf(args[2], "%zu", &datasize)) {
                printf("Execute Read at %zu, %zu bytes\n", address, datasize);
                struct data_read *d_r = generate_data_read(address, datasize);
                send_command(OP_READ, d_r, leader);
                free(d_r);
            } else {
                // Read to File
                FILE *file_read = fopen(args[2], "w");
                if (!file_read) {
                    error_msg("r 'address' 'file', file was not possible to open, or doesn't exist");
                    return;
                }
                struct stat st;
                if (stat(args[2], &st) != 0)
                    return;
                if (l == 4) {
                    // file max data read
                    if (1 == sscanf(args[3], "%zu", &datasize)) {
                        if ((ssize_t) datasize >= st.st_size)
                            datasize = st.st_size;
                    } else {
                        error_msg("r requires an argument 'datasize' which can be casted as a positive integer");
                        return;
                    }
                } else {
                    datasize = st.st_size;
                }
                // Now read datasize bytes
                char *buffer_read_file = NULL;
                struct data_write *d_w = generate_data_write(address, datasize, buffer_read_file);
                send_command(OP_READ_FILE, d_w, leader);
                // /!\ buffer_read_file was allocated in send_command
                debug("Write data in file (READ OP)", 0);
                printf("Data Read :: %zu", d_w->size);
                size_t r_c = fwrite(d_w->data, sizeof(char), d_w->size, file_read);
                if (r_c != d_w->size)
                    error_msg("WARNING: file bytes write different from bytes asked");
                // OLD error_msg("r requires an argument 'datasize' which can be casted as a positive integer");
            }
        } else {
            error_msg("r require an argument 'address' which can be casted as a positive integer");
        }
    } else if (0 == strcmp(args[0], "d")) {
        // ERRORS
        if (l <= 1) {
            error_msg("d requires minimum 1 argument and maximum 2 : 'address' | 'net and 'file'");
            return;
        } else if (l >= 4) {
            error_msg("d do not support more than 2 arguments, check command h");
            return;
        }

        // Execution
        size_t address = 0;
        if (0 == strcmp(args[1], "net")) {
            printf("Execute DUMP_ALL");
            send_command(OP_DUMP_ALL, NULL, leader);
        } else if (1 == sscanf(args[1], "%zu", &address)) {

            // d address
            if (l == 2) {
                printf("Execute Dump of %zu\n", address);
                struct data_address *d_a = generate_data_address(address);
                send_command(OP_DUMP, d_a, leader);
                free(d_a);
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
    } else if (0 == strcmp(args[0], "t")) {
        // ERRORS
        if (l != 1) {
            error_msg("t require no arguments");
            return;
        }
        send_command(OP_TABLE, NULL, leader);

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
    } else {
        error_msg("command not found, see 'h' for help");
    }

}

unsigned short start_leader_election(unsigned size) {
    struct message *b = generate_message(0, 0, 0, 0, 0, OP_START_LEADER);
    broadcast_message(b, 0, size, TAG_MSG);
    free(b);
    struct message *m = generate_message(0, 0, 0, 0, 0, OP_NONE);
    MPI_Recv(m, sizeof(*m), MPI_BYTE, MPI_ANY_SOURCE, TAG_MSG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    debug("Got Leader !", DEF_NODE_USER);
    unsigned short leader = m->id_o;
    free(m);
    return leader;
}

void start_cli(unsigned size) {
    int no_quit = 1;
    char *cmd;
    char **args;

    struct queue *q = queue_init();
    unsigned short leader = start_leader_election(size);

    while (no_quit) {
        fflush(0);
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
        struct message *m_verif = generate_message_a(0, leader, 0, 0, 0, OP_IS_ALIVE, 1);
        if (!send_safe_message(m_verif, q, TAG_MSG))
            leader = start_leader_election(size);
        free(m_verif);
        execute(args, leader);

        free(cmd);
        free(args);
    }
}
