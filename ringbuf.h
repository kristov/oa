#ifndef RINGBUF_H
#define RINGBUF_H

#include <stdint.h>

struct rb {
    uint8_t mask;
    uint8_t write;
    uint8_t read;
    uint8_t buffer[16];
};

uint8_t rb_read(struct rb* buff, uint8_t* c);

uint8_t rb_write(struct rb* buff, uint8_t c);

uint8_t rb_init(struct rb* buff, uint8_t mask);

#endif
