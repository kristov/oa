#include <shell.h>
#include <mini8fs.h>
#include <queue.h>
#include <uart.h>
#include <avr/pgmspace.h>

#define CHAR_NEWLINE 0x0a
#define CHAR_CRETURN 0x0d
#define CHAR_BSPACE 0x7f

struct shell* SHP;

const struct shell_bi builtins[] PROGMEM = {
    {(uint8_t*)"ls", shell_bi_ls}
};

uint8_t shell_bi_ls(uint8_t argc, char** argv) {
    return 1;
}

void shell_prompt(struct rb* sob) {
    rb_write(sob, '$');
    rb_write(sob, ' ');
}

uint8_t shell_streq(uint8_t* str1, uint8_t* str2) {
    while (*str1 && *str2) {
        if (*str1 != *str2) {
            return 1;
        }
        str1++;
        str2++;
    }
    if ((*str1 == 0) && (*str2 == 0)) {
        return 0;
    }
    return 1;
}

uint8_t shell_interpret(struct rb* sob) {
    // https://gist.github.com/parse/966049
    uint8_t start = 0;
    uint8_t end = 0;
    for (end = 0; end < SHP->write; end++) {
        if (SHP->linebuff[end] == 0) {
            break;
        }
    }
    if (start == end) {
        return 1;
    }
    uint8_t i = 0;
    struct shell_bi bi;
    for (i = 0; i < 1; i++) {
        bi = builtins[i];
        if (shell_streq(bi.cmd, &SHP->linebuff[start])) {
            continue;
        }
        bi.handler(0, 0);
    }
    return 0;
}

uint8_t shell_si_consumer(struct rb* buff) {
    uint8_t c;
    struct queue* so = SHP->so;
    struct rb* sob = &so->buff;
    if (rb_read(buff, &c)) {
        return 1;
    }
    uint8_t space = rb_space(sob);
    if (c == CHAR_CRETURN) {
        if (space < 4) {
            return 1;
        }
        rb_write(sob, CHAR_NEWLINE);
        rb_write(sob, CHAR_CRETURN);
        uint8_t ret = shell_interpret(sob);
        shell_prompt(sob);
        SHP->write = 0;
        return ret;
    }
    if (c == CHAR_BSPACE) {
        if (SHP->write == 0) {
            return 1;
        }
        if (space < 7) {
            return 1;
        }
        SHP->linebuff[SHP->write] = 0;
        SHP->write--;
        rb_write(sob, 0x1b);
        rb_write(sob, 0x5b);
        rb_write(sob, 0x44);
        rb_write(sob, 0x20);
        rb_write(sob, 0x1b);
        rb_write(sob, 0x5b);
        rb_write(sob, 0x44);
        return 0;
    }
    if (SHP->write == SHELL_LINEBUF_LIMIT) {
        return 0;
    }
    SHP->linebuff[SHP->write] = c;
    SHP->write++;
    SHP->linebuff[SHP->write] = 0;
    return rb_write(sob, c);
}

uint8_t shell_so_producer(struct rb* buff) {
    return 0;
}

uint8_t shell_init() {
    struct shell* sh = (struct shell*)m8_newfile(0, (uint8_t*)"proc/shell", 64);
    if (!sh) {
        uart_println((uint8_t*)"ERR: unable to create proc/shell");
        return 1;
    }
    SHP = sh;
    SHP->write = 0;
    SHP->read = 0;

    struct queue* si = (struct queue*)m8_open(0, (uint8_t*)"dev/stdin");
    if (!si) {
        uart_println((uint8_t*)"ERR: unable to find dev/stdin");
        return 1;
    }
    si->consumer = shell_si_consumer;
    SHP->si = si;

    struct queue* so = (struct queue*)m8_open(0, (uint8_t*)"dev/stdout");
    if (!so) {
        uart_println((uint8_t*)"ERR: unable to find dev/stdout");
        return 1;
    }
    so->producer = shell_so_producer;
    SHP->so = so;

    shell_prompt(&so->buff);

    return 0;
}
