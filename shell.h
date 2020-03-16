#ifndef SHELL_H
#define SHELL_H

#include <stdint.h>

struct shell {
    struct queue* si;
    struct queue* so;
    uint8_t linebuff[60];
};

uint8_t shell_init();

#endif
