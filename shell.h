#ifndef SHELL_H
#define SHELL_H

#include <stdint.h>

#define SHELL_LINEBUF_SIZE 58
#define SHELL_LINEBUF_LIMIT (SHELL_LINEBUF_SIZE - 3)

typedef uint8_t (*cmdh)(uint8_t argc, char** argv);

struct shell {
    struct queue* si;
    struct queue* so;
    uint8_t write;
    uint8_t read;
    uint8_t linebuff[SHELL_LINEBUF_SIZE];
};

struct shell_bi {
    uint8_t* cmd;
    cmdh handler;
};

uint8_t shell_bi_ls(uint8_t argc, char** argv);

uint8_t shell_init();

#endif
