#include <ringbuf.h>

uint8_t rb_read(struct rb* buff, uint8_t* c) {
    uint8_t read = buff->read;
    if (buff->write == read) {
        return 1;
    }
    *c = buff->buffer[read];
    read++;
    read = read & buff->mask;
    buff->read = read;
    return 0;
}

uint8_t rb_write(struct rb* buff, uint8_t c) {
    uint8_t write = buff->write;
    write++;
    write = write & buff->mask;
    if (write == buff->read) {
        return 1;
    }
    buff->buffer[buff->write] = c;
    buff->write = write;
    return 0;
}

uint8_t rb_space(struct rb* buff) {
    if (buff->read > buff->write) {
        return (buff->read - buff->write) - 1;
    }
    return buff->write - buff->read;
}

uint8_t rb_init(struct rb* buff, uint8_t mask) {
    buff->mask = mask;
    buff->read = 0;
    buff->write = 0;
    return 0;
}
