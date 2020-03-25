#include <uart.h>
#include <mini8fs.h>
#include <shell.h>
#include <queue.h>

uint8_t fs_init() {
    uint8_t* addr;
    addr = m8_mkdir(0, (uint8_t*)"dev");
    if (!addr) {
        uart_println((uint8_t*)"ERR: unable to make dev dir");
        return 1;
    }
    addr = m8_mkdir(0, (uint8_t*)"proc");
    if (!addr) {
        uart_println((uint8_t*)"ERR: unable to make proc dir");
        return 1;
    }
    return 0;
}

uint8_t init() {
    if (m8_init()) {
        return 1;
    }
    if (fs_init()) {
        return 1;
    }
    if (uart_init()) {
        return 1;
    }
    if (shell_init()) {
        return 1;
    }
    return 0;
}

void loop_forever() {
    while (1) {
    }
}

int main () {
    if (init()) {
        uart_println((uint8_t*)"ERR: init() failed");
    }
    uart_println((uint8_t*)"oa: v0.1");
    struct queue* si = (struct queue*)m8_open(0, (uint8_t*)"dev/stdin");
    if (!si) {
        uart_println((uint8_t*)"ERR: unable to find dev/stdin");
        loop_forever();
    }
    struct queue* so = (struct queue*)m8_open(0, (uint8_t*)"dev/stdout");
    if (!so) {
        uart_println((uint8_t*)"ERR: unable to find dev/stdout");
        loop_forever();
    }

    while (1) {
        queue_process(si);
        queue_process(so);
    }
}
