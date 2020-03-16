#ifndef QUEUE_H
#define QUEUE_H

#include <ringbuf.h>

typedef uint8_t (*qcb)(struct rb* buff);

struct queue {
    qcb producer;
    qcb consumer;
    struct rb buff;
};

uint8_t queue_process(struct queue* q);

uint8_t queue_init(struct queue* q, qcb producer, qcb consumer);

#endif
