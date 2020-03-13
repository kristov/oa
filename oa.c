#include <ringbuf.h>
#include <uart.h>

typedef uint8_t (*qcallback)(struct rb* buff);

#define QMASK 0x0f
struct queue {
    qcallback producer;
    qcallback consumer;
};

struct queue queues[2];
struct rb rbs[2];

uint8_t boot[] = "oa: v0.1";

void println(uint8_t* str) {
    while (*str) {
        while (uart_putc(*str));
        str++;
    }
    while (uart_putc('\n'));
    while (uart_putc('\r'));
}

uint8_t stdin_producer(struct rb* buff) {
    uint8_t c;
    if (uart_getc(&c)) {
        return 1;
    }
    return rb_write(buff, c);
}

uint8_t stdin_consumer(struct rb* buff) {
    uint8_t c;
    if (rb_read(buff, &c)) {
        return 1;
    }
    return rb_write(&rbs[1], c);
}

uint8_t stdout_producer(struct rb* buff) {
    return 0;
}

uint8_t stdout_consumer(struct rb* buff) {
    uint8_t c;
    if (rb_read(buff, &c)) {
        return 1;
    }
    while (uart_putc(c));
    return 0;
}

void queue_init(uint8_t queue_id, qcallback producer, qcallback consumer) {
    rb_init(&rbs[queue_id], QMASK);
    struct queue* q = &queues[queue_id];
    q->producer = producer;
    q->consumer = consumer;
}

uint8_t queue_process(uint8_t queue_id) {
    struct queue* q = &queues[queue_id];
    qcallback producer = q->producer;
    if (producer(&rbs[queue_id])) {
        return 1;
    }
    qcallback consumer = q->consumer;
    return consumer(&rbs[queue_id]);
}

int main () {
    uart_init();
    println(&boot[0]);
    queue_init(0, stdin_producer, stdin_consumer);
    queue_init(1, stdout_producer, stdout_consumer);
    uint8_t i = 0;
    while (1) {
        for (i = 0; i < 2; i++) {
            queue_process(i);
        }
    }
}
