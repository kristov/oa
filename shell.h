#ifndef SHELL_H
#define SHELL_H

#include <stdint.h>

#define SHELL_LINEBUF_SIZE 58
#define SHELL_LINEBUF_LIMIT (SHELL_LINEBUF_SIZE - 3)

struct shell {
    struct queue* si;
    struct queue* so;
    uint8_t write;
    uint8_t read;
    uint8_t linebuff[SHELL_LINEBUF_SIZE];
};

uint8_t shell_init();

#endif
