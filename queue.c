#include <queue.h>

uint8_t queue_process(struct queue* q) {
    qcb callback = 0;
    callback = q->producer;
    if (callback) {
        if (callback(&q->buff)) {
            return 1;
        }
    }
    callback = q->consumer;
    if (callback) {
        return callback(&q->buff);
    }
    return 1;
}

uint8_t queue_init(struct queue* q, qcb producer, qcb consumer) {
    q->producer = producer;
    q->consumer = consumer;
    return rb_init(&q->buff, 0x0f);
}
