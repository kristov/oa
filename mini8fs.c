#include <mini8fs.h>

uint8_t memory[2048];

uint8_t* m8_blk_addr(uint8_t blockid) {
    return &memory[(blockid * M8_BLOCK_SIZE) + M8_FT_SIZE];
}

uint8_t* m8_blkc_find(uint8_t blockid, uint8_t* name, uint8_t strlen) {
    while (blockid) {
        uint8_t* blkaddr = m8_blk_addr(blockid);
        uint8_t b = M8_FILES_PER_BLOCK;
        while (b) {
            uint8_t* str1 = blkaddr;
            uint8_t* str2 = name;
            uint8_t eq = 1;
            uint8_t c = M8_FNAME_LEN;
            while (c) {
                if (*str1 != *str2) {
                    eq = 0;
                    break;
                }
                c--;
            }
            if (eq) {
                return blkaddr;
            }
            b--;
        }
        blockid = memory[(blockid * 2) + 1];
    }
    return 0;
}
