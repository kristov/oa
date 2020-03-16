#ifndef QUEUE_H
#define QUEUE_H

#include <ringbuf.h>

struct queue {
    rbcb producer;
    rbcb consumer;
    struct rb buff;
};

uint8_t queue_process(struct queue* q);

uint8_t queue_init(struct queue* q, rbcb producer, rbcb consumer);

#endif
