#include <ringbuf.h>
#include <uart.h>

void init() {
    uart_init();
    m8_init();
    shell_init();
}

int main () {
    init();
    uart_println("oa: v0.1");
    while (1) {
    }
}
