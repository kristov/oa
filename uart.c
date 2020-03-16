#include <uart.h>
#include <mini8fs.h>
#include <queue.h>

#define F_CPU 16000000UL
#define BAUD 57600
#include <util/setbaud.h>

void uart_println(uint8_t* str) {
    while (*str) {
        while (uart_putc(*str));
        str++;
    }
    while (uart_putc('\n'));
    while (uart_putc('\r'));
}

uint8_t uart_producer(struct rb* buff) {
    uint8_t c;
    if (uart_getc(&c)) {
        return 1;
    }
    return rb_write(buff, c);
}

uint8_t uart_consumer(struct rb* buff) {
    uint8_t c;
    if (rb_read(buff, &c)) {
        return 1;
    }
    while (uart_putc(c));
    return 0;
}

uint8_t uart_putc(uint8_t c) {
    if (!(UCSR0A & (1 << UDRE0))) {
        return 1;
    }
    UDR0 = c;
	return 0;
}

uint8_t uart_getc(uint8_t* c) {
	if (!(UCSR0A & (1 << RXC0))) {
        return 1;
    }
    *c = UDR0;
	return 0;
}

uint8_t uart_init() {
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
#if USE_2X
    UCSR0A |= _BV(U2X0);
#else
    UCSR0A &= ~(_BV(U2X0));
#endif
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);

    uint8_t* si = m8_newfile(0, (uint8_t*)"dev/stdin", 32);
    if (!si) {
        uart_println((uint8_t*)"ERR: unable to make dev/stdin");
        return 1;
    }
    uint8_t* so = m8_newfile(0, (uint8_t*)"dev/stdout", 32);
    if (!so) {
        uart_println((uint8_t*)"ERR: unable to make dev/stdout");
        return 1;
    }

    queue_init((struct queue*)si, uart_producer, 0);
    queue_init((struct queue*)so, 0, uart_consumer);

    return 0;
}
