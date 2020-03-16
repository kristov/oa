#ifndef UART_H
#define UART_H

#include <avr/io.h>

void uart_println(uint8_t* str);

uint8_t uart_putc(uint8_t c);

uint8_t uart_getc(uint8_t* c);

uint8_t uart_init();

#endif
