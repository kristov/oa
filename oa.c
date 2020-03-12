#include <ringbuf.h>
#include <uart.h>

//ISR(USART_RX_vect) {
//    uart_interrupt();
//}

int main () {
    uint8_t c;
    uart_init(9600);
    for (;;) {
        if (uart_getc(&c)) {
            continue;
        }
        while (uart_putc(c));
    }
}
