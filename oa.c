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
    return shell_init();
}

int main () {
    init();
    uart_println((uint8_t*)"oa: v0.1");
    struct queue* si = (struct queue*)m8_path_find(0, (uint8_t*)"dev/stdin");
    struct queue* so = (struct queue*)m8_path_find(0, (uint8_t*)"dev/stdout");
    while (1) {
        queue_process(si);
        queue_process(so);
    }
}
