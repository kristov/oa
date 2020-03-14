#ifndef MINI8FS_H
#define MINI8FS_H

#include <common.h>

#define M8_NR_BLOCKS 32
#define M8_BLOCK_SIZE 32
#define M8_FNAME_LEN 6

#define M8_NR_BLOCKS_MASK (M8_NR_BLOCKS - 1)
#define M8_FT_SIZE (M8_NR_BLOCKS * 2)
#define M8_FENTRY_LEN (M8_FNAME_LEN + 2)
#define M8_FILES_PER_BLOCK (M8_BLOCK_SIZE / M8_FENTRY_LEN)

extern uint8_t m8_memory[1088];

uint8_t* m8_blk_addr(uint8_t blockid);

uint8_t* m8_blkc_find(uint8_t blockid, uint8_t* name, uint8_t strlen);

uint8_t m8_find_cons_blks(uint8_t nrblocks);

#endif
