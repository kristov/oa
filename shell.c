#include <shell.h>
#include <mini8fs.h>
#include <queue.h>
#include <uart.h>

#define CHAR_NEWLINE 0x0a
#define CHAR_CRETURN 0x0d
#define CHAR_BSPACE 0x7f

struct shell* SHP;

void shell_prompt() {
    SHP->linebuff[SHP->write] = '$';
    SHP->write++;
    SHP->linebuff[SHP->write] = ' ';
    SHP->write++;
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
//            shell_println(sob, &SHP->linebuff[sidx]);
            sidx = idx + 1;
        }
    }
    if (sidx == idx) {
        return 0;
    }
//    shell_println(sob, &SHP->linebuff[sidx]);
    return 0;
}

uint8_t shell_si_consumer(struct rb* buff) {
    uint8_t c;
    struct queue* so = SHP->so;
    struct rb* sob = &so->buff;
    if (rb_read(buff, &c)) {
        return 1;
    }
    if (c == CHAR_CRETURN) {
        uint8_t ret = shell_interpret(sob);
        SHP->write = 0;
        SHP->read = 0;
        SHP->linebuff[SHP->write] = CHAR_NEWLINE;
        SHP->write++;
        SHP->linebuff[SHP->write] = CHAR_CRETURN;
        SHP->write++;
        SHP->linebuff[SHP->write] = 0;
        SHP->write++;
        return ret;
    }
    if (c == CHAR_BSPACE) {
        if (SHP->write == 0) {
            return 1;
        }
        SHP->linebuff[SHP->write] = 0;
        if (SHP->write == SHP->read) {
            SHP->read--;
        }
        SHP->write--;
        rb_write(&so->buff, 0x1b);
        rb_write(&so->buff, 0x5b);
        rb_write(&so->buff, 0x44);
        rb_write(&so->buff, 0x20);
        rb_write(&so->buff, 0x1b);
        rb_write(&so->buff, 0x5b);
        rb_write(&so->buff, 0x44);
        return 0;
    }
    if (SHP->write == SHELL_LINEBUF_LIMIT) {
        return 0;
    }
    SHP->linebuff[SHP->write] = c;
    SHP->write++;
    return 0;
}

uint8_t shell_so_producer(struct rb* buff) {
    if (SHP->write == SHP->read) {
        return 0;
    }
    uint8_t ret = rb_write(buff, SHP->linebuff[SHP->read]);
    if (ret) {
        return 1;
    }
    SHP->read++;
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
