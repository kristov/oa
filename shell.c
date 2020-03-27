#include <shell.h>
#include <mini8fs.h>
#include <queue.h>
#include <uart.h>

struct shell* SHP;

uint8_t shell_putc(uint8_t c) {
    struct queue* so = SHP->so;
    return rb_write(&so->buff, c);
}

uint8_t shell_si_consumer(struct rb* buff) {
    uint8_t c;
    if (rb_read(buff, &c)) {
        return 1;
    }
    if (c == '\n') {
        SHP->linebuff[SHP->idx] = 0;
        SHP->idx = 0;
        // interpret the line buffer contents
    }
    if (SHP->idx == 58) {
        return 1;
    }
    SHP->linebuff[SHP->idx] = c;
    SHP->idx++;
    return shell_putc(c);
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
    SHP->idx = 0;

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

    return 0;
}
