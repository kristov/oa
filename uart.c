#include <uart.h>

void uart_init(uint32_t baud) {
	UCSR0A = (1 << U2X0);
	UCSR0B = (0 << RXCIE0) |
             (0 << TXCIE0) |
             (0 << UDRIE0) |
             (0 << RXEN0) |
             (0 << TXEN0) |
             (0 << UCSZ02) |
             (0 << RXB80) |
             (0 << TXB80);
	UCSR0C = (0 << UMSEL01) |
             (0 << UMSEL00) |
             (0 << UPM01) |
             (0 << UPM00) |
             (0 << USBS0) |
             (1 << UCSZ01) |
             (1 << UCSZ00) |
             (1 << UCPOL0);
	UBRR0 = F_CPU / ((UCSR0A & (1 << U2X0)) ? 8 : 16) / baud - 1;
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
