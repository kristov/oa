#include <blk.h>
#include <stdio.h>
#include <string.h>

void readfs(char *filename) {
    FILE* f = fopen(filename, "r");
    fread(&blk_memory, 1, 1088, f);
    fclose(f);
}

uint8_t is_u8(uint8_t got, uint8_t exp, char* msg) {
    if (got == exp) {
        printf("ok: \"%s\"\n", msg);
        return 1;
    }
    printf("bad: got %d, exp %d \"%s\"\n", got, exp, msg);
    return 0;
}

uint8_t is_null(uint8_t* got, char* msg) {
    if (got == NULL) {
        printf("ok: \"%s\"\n", msg);
        return 1;
    }
    printf("bad: not NULL \"%s\"\n", msg);
    return 0;
}

uint8_t is_not_null(uint8_t* got, char* msg) {
    if (got != NULL) {
        printf("ok: \"%s\"\n", msg);
        return 1;
    }
    printf("bad: got NULL \"%s\"\n", msg);
    return 0;
}

uint8_t test_blk_functions() {
    uint8_t pass = 0;

    uint8_t blkid0 = blk_find_cons_blks(1);
    pass += is_u8(blkid0, 7, "found 1 free block at 7");

    uint8_t blkid1 = blk_find_cons_blks(2);
    pass += is_u8(blkid1, 7, "found 2 free blocks at 7");

    uint8_t blkid2 = blk_find_cons_blks(3);
    pass += is_u8(blkid2, 10, "found 3 free blocks at 10");

    uint8_t blkid3 = blk_find_cons_blks(31);
    pass += is_u8(blkid3, 0, "no free blocks found");

    uint8_t* addr3 = blk_link_cons_blks(7, 2);
    pass += is_not_null(addr3, "found address for linked blocks");
    pass += is_u8(blk_memory[14], 1, "chunk 0,0 written correctly");
    pass += is_u8(blk_memory[15], 8, "chunk 0,1 written correctly");
    pass += is_u8(blk_memory[16], 1, "chunk 1,0 written correctly");
    pass += is_u8(blk_memory[17], 0, "chunk 1,1 written correctly");

    uint8_t* addr4 = blk_extend(9, 6);
    pass += is_not_null(addr4, "found address for extended blocks");
    pass += is_u8(blk_memory[19], 10, "block linked to previous block");
    pass += is_u8(blk_memory[20], 1, "chunk 0,0 written correctly");
    pass += is_u8(blk_memory[21], 11, "chunk 0,1 written correctly");
    pass += is_u8(blk_memory[28], 1, "chunk 4,0 written correctly");
    pass += is_u8(blk_memory[29], 15, "chunk 4,0 written correctly");
    pass += is_u8(blk_memory[30], 1, "chunk 5,0 written correctly");
    pass += is_u8(blk_memory[31], 0, "chunk 5,0 written correctly");

    uint8_t* addr5 = blk_extend(9, 1);
    pass += is_not_null(addr5, "found address for extended blocks");
    pass += is_u8(blk_memory[31], 20, "block linked to previous block");
    pass += is_u8(blk_memory[40], 1, "chunk 0,0 written correctly");
    pass += is_u8(blk_memory[41], 0, "chunk 0,0 written correctly");

    uint8_t unlinked = blk_unlink_cons_blks(2);
    pass += is_u8(unlinked, 2, "correct number blocks unlinked");
    pass += is_u8(blk_memory[4], 0, "chunk 0,0 cleared correctly");
    pass += is_u8(blk_memory[5], 0, "chunk 0,1 cleared correctly");
    pass += is_u8(blk_memory[6], 0, "chunk 1,0 cleared correctly");
    pass += is_u8(blk_memory[7], 0, "chunk 1,1 cleared correctly");

    return pass;
}

int main() {
    readfs("fsimg.bin");
    test_blk_functions();
    return 0;
}
