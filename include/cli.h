#ifndef DISTRIBUTEDMALLOC_CLI_H
#define DISTRIBUTEDMALLOC_CLI_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <err.h>

#include "message.h"
#include "utils.h"

void start_cli(unsigned size);

void send_command(enum operation op, void *data, unsigned short leader);

#endif /* !DISTRIBUTEDMALLOC_CLI_H */
