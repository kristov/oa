#include <mini8fs.h>

uint8_t m8_memory[1088];

uint8_t* m8_blk_addr(uint8_t blockid) {
    return &m8_memory[(blockid * M8_BLOCK_SIZE) + M8_FT_SIZE];
}

uint8_t* m8_blkc_find(uint8_t blockid, uint8_t* name, uint8_t strlen) {
    do {
        uint8_t* blkaddr = m8_blk_addr(blockid);
        uint8_t b = M8_FILES_PER_BLOCK;
        while (b) {
            uint8_t* str1 = blkaddr;
            uint8_t* str2 = name;
            uint8_t eq = 1;
            uint8_t c = strlen;
            while (c) {
                if (*str1 != *str2) {
                    eq = 0;
                    break;
                }
                str1++;
                str2++;
                c--;
            }
            if (eq) {
                return blkaddr;
            }
            blkaddr += M8_FENTRY_LEN;
            b--;
        }
        blockid = m8_memory[(blockid * 2) + 1];
    } while (blockid);
    return 0;
}

uint8_t m8_find_cons_blks(uint8_t nrblocks) {
    uint8_t bs = 0;
    uint8_t bc = 0;
    uint8_t nb = nrblocks;
    do {
        if (m8_memory[bc * 2]) {
            nb = nrblocks;
            bc++;
            bc = bc & M8_NR_BLOCKS_MASK;
            bs = bc;
            continue;
        }
        nb--;
        if (!nb) {
            return bs;
        }
        bc++;
        bc = bc & M8_NR_BLOCKS_MASK;
    } while (bc);
    return 0;
}
