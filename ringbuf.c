#include <ringbuf.h>

uint8_t rb_read(struct rb* buff, uint8_t* c) {
    uint8_t read = buff->read;
    if (buff->write == read) {
        return 0;
    }
    *c = buff->buffer[read];
    read++;
    read = read & buff->mask;
    buff->read = read;
    return 1;
}

uint8_t rb_write(struct rb* buff, uint8_t c) {
    uint8_t write = buff->write;
    write++;
    write = write & buff->mask;
    if (write == buff->read) {
        return 0;
    }
    buff->buffer[write] = c;
    buff->write = write;
    return 1;
}
