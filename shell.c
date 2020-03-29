#include <shell.h>
#include <mini8fs.h>
#include <queue.h>
#include <uart.h>

struct shell* SHP;

uint8_t shell_prompt(struct rb* sob) {
    rb_write(sob, (uint8_t)'$');
    return rb_write(sob, (uint8_t)' ');
}

uint8_t shell_println(struct rb* sob, uint8_t* str) {
    uint8_t c = *str;
    while (c) {
        rb_write(sob, c);
        str++;
        c = *str;
    }
    rb_write(sob, (uint8_t)'\n');
    return rb_write(sob, (uint8_t)'\r');
}

uint8_t shell_interpret(struct rb* sob) {
    // interpret the line buffer contents
    // https://gist.github.com/parse/966049
    uint8_t idx = 0;
    uint8_t sidx = 0;
    for (idx = 0; idx < SHP->write; idx++) {
        if (SHP->linebuff[idx] == ' ') {
            SHP->linebuff[idx] = 0;
            if (sidx == idx) {
                continue;
            }
            shell_println(sob, &SHP->linebuff[sidx]);
            sidx = idx + 1;
        }
    }
    if (sidx == idx) {
        return 0;
    }
    shell_println(sob, &SHP->linebuff[sidx]);
    return 0;
}

uint8_t shell_si_consumer(struct rb* buff) {
    uint8_t c;
    struct queue* so = SHP->so;
    struct rb* sob = &so->buff;
    if (rb_read(buff, &c)) {
        return 1;
    }
    if (c == 0x0d) { // r
        SHP->linebuff[SHP->write] = 0;
        rb_write(sob, (uint8_t)'\n');
        rb_write(sob, (uint8_t)'\r');
        uint8_t ret = shell_interpret(sob);
        SHP->write = 0;
        SHP->read = 0;
        shell_prompt(sob);
        return ret;
    }
    if (c == 0x7f) {
        SHP->linebuff[SHP->write] = 0;
        SHP->write--;
        SHP->read = 0;
        rb_write(sob, (uint8_t)'\r');
    }
    if (SHP->write == SHELL_LINEBUF_LIMIT) {
        return 1;
    }
    SHP->linebuff[SHP->write] = c;
    SHP->write++;
    return 0;
}

uint8_t shell_so_producer(struct rb* buff) {
    if (SHP->write == SHP->read) {
        return 1;
    }
    uint8_t ret = rb_write(buff, SHP->linebuff[SHP->read]);
    SHP->read++;
    return ret;
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
