#ifndef UART_H
#define UART_H

#include <avr/io.h>

void uart_init();

uint8_t uart_putc(uint8_t c);

uint8_t uart_getc(uint8_t* c);

#endif
