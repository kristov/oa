#include <blk.h>

uint8_t blk_memory[BLK_MEM_SIZE];

uint8_t blk_find_cons_blks(uint8_t nrblks) {
    uint8_t bs = 0;
    uint8_t bc = 0;
    uint8_t nb = nrblks;
    do {
        if (blk_memory[bc * 2]) {
            nb = nrblks;
            bc++;
            bc = bc & BLK_NR_BLOCKS_MASK;
            bs = bc;
            continue;
        }
        nb--;
        if (!nb) {
            return bs;
        }
        bc++;
        bc = bc & BLK_NR_BLOCKS_MASK;
    } while (bc);
    return 0;
}

uint8_t* blk_link_cons_blks(uint8_t blkid, uint8_t nrblks) {
    uint8_t* addr = BLK_ADDR(blkid);
    uint8_t i = blkid * 2;
    while (nrblks) {
        blkid++;
        blk_memory[i] = 1; i++;
        nrblks--;
        if (nrblks) {
            blk_memory[i] = blkid; i++;
        }
    };
    return addr;
}

uint8_t blk_unlink_cons_blks(uint8_t blkid) {
    uint8_t c = 0;
    uint8_t i = 0;
    while (blkid) {
        i = blkid * 2;
        blk_memory[i] = 0; i++;
        blkid = blk_memory[i];
        blk_memory[i] = 0;
        c++;
    };
    return c;
}

uint8_t* blk_extend(uint8_t blockid, uint8_t nrblocks) {
    uint8_t lblockid;
    do {
        lblockid = blockid;
        blockid = blk_memory[(blockid * 2) + 1];
    } while (blockid);
    blockid = blk_find_cons_blks(nrblocks);
    if (!blockid) {
        return 0;
    }
    blk_memory[(lblockid * 2) + 1] = blockid;
    return blk_link_cons_blks(blockid, nrblocks);
}

uint8_t blk_init() {
    uint16_t i = 0;
    for (i = 0; i < BLK_MEM_SIZE; i++) {
        blk_memory[i] = 0;
    }
    blk_memory[0] = 1;
    return 0;
}
