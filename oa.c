#include <ringbuf.h>
#include <uart.h>

uint8_t c;

//ISR(USART_RX_vect) {
//    uart_getc(&c);
//    uart_putc(c);
//}

int main () {
    uart_init();
    while (1) {
        if (uart_getc(&c)) {
            continue;
        }
        while (uart_putc(c));
    }
}
