#include <uart.h>
#include <mini8fs.h>
#include <shell.h>
#include <queue.h>

struct queue* QUEUE[10];

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

uint8_t oa_init() {
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

uint8_t* oa_fill_dev(uint8_t* entry, void* usr) {
    uint8_t* idx = (uint8_t*)usr;
    uint8_t eblkid = entry[M8_BLOCKID_BYTE];
    if (entry[M8_STATUS_BYTE] != 0x7f) {
        return 0;
    }
    QUEUE[*idx] = (struct queue*)M8_BLK_ADDR(eblkid);
    *idx = *idx + 1;
    return 0;
}

uint8_t oa_init_queues() {
    uint8_t idx = 0;
    for (idx = 0; idx < 10; idx++) {
        QUEUE[idx] = 0;
    }
    uint8_t* dev = m8_path_find(0, (uint8_t*)"dev");
    if (!dev) {
        return 1;
    }
    uint8_t devblkid = dev[M8_BLOCKID_BYTE];
    if (!devblkid) {
        return 1;
    }
    idx = 0;
    m8_blkc_iter(devblkid, oa_fill_dev, (void*)&idx);
    return 0;
}

int main () {
    uart_println((uint8_t*)"oa: v0.1");
    if (oa_init()) {
        uart_println((uint8_t*)"ERR: init() failed");
    }
    if (oa_init_queues()) {
        uart_println((uint8_t*)"ERR: init() failed");
    }
    uint8_t idx = 0;
    while (1) {
        for (idx = 0; idx < 10; idx++) {
            if (!QUEUE[idx]) {
                break;
            }
            queue_process(QUEUE[idx]);
        }
    }
    return 0;
}
