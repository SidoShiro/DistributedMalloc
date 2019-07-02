#include "debug.h"

void debug(char *msg, unsigned short id) {
    /*
     [0;31m	Red
[1;31m	Bold Red
[0;32m	Green
[1;32m	Bold Green
[0;33m	Yellow
[01;33m	Bold Yellow
[0;34m	Blue
[1;34m	Bold Blue
[0;35m	Magenta
[1;35m	Bold Magenta
[0;36m	Cyan
[1;36m	Bold Cyan
[0m	Reset
     */
    unsigned nb_vars = 12;
    char reset[] = "\033[0m\0";
    char *colors[] = {
            "\033[0;31m",
            "\033[0;32m",
            "\033[0;33m",
            "\033[0;34m",
            "\033[0;35m",
            "\033[0;36m",
            "\033[1;31m",
            "\033[1;32m",
            "\033[1;33m",
            "\033[1;34m",
            "\033[1;35m",
            "\033[1;36m"
    };
    if (id == 0) {
        printf("\033[1;0mu: %s\033[0m\n", msg);
        return;
    }
    unsigned short c = id % nb_vars;
    printf("%s%d: %s%s\n", colors[c], id, msg, reset);
}
